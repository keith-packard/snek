/*
 * Copyright © 2025 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>
#include "snek-70909.h"

static uint32_t
milliseconds(void)
{
	struct timespec t;
	(void) clock_gettime(CLOCK_MONOTONIC, &t);
	return (uint32_t) (t.tv_sec * 1000 + t.tv_nsec / 1000000);
}

#define KEEPALIVE_MS	1000
#define TIMEOUT_MS	100

static void
lego_lock(lego_buf_t *l)
{
	int	ret = pthread_mutex_lock(&l->mutex);
	if (ret) {
		abort();
	}
}

static void
lego_unlock(lego_buf_t *l)
{
	int	ret = pthread_mutex_unlock(&l->mutex);
	if (ret) {
		abort();
	}
}

/* Fill the input buffer. Timeout if there is nothing available */
static int
lego_fill(lego_buf_t *l, int timeout)
{
	struct pollfd	fds[1] = {
		{ .fd = l->fd, .events = POLLIN | POLLERR | POLLHUP, .revents = 0 }
	};
	int ready;

	/* check for readable */
	ready = poll(fds, 1, timeout);
	if (ready == 0) {
		return LEGO_TIMEOUT;
	}
	if (ready < 0) {
		return LEGO_ERR;
	}

	ssize_t count;
	count = read(l->fd, l->buf, sizeof(l->buf));
	if (count <= 0)
		return LEGO_ERR;

	l->count = count;
	l->used = 0;
	return LEGO_OK;
}

/* Discard any pending input to re-synchronize the device */
static void
lego_flush(lego_buf_t *l)
{
	int ret;

	for (;;) {
		l->used = l->count = 0;
		ret = lego_fill(l, 0);
		if (ret != LEGO_OK)
			break;
	}
}

/* Get a single byte from the device */
static int
lego_getc(lego_buf_t *l)
{
	int	ret;

	if (l->used == l->count) {
		ret = lego_fill(l, TIMEOUT_MS);
		if (ret != LEGO_OK)
			return ret;
	}
	return (unsigned char) l->buf[l->used++];
}

/* Wait for the device to report back the startup string */
static int
lego_read_until(lego_buf_t *l, char *buf, size_t len, const char *start, const char *end)
{
	size_t	start_len = strlen(start);
	size_t	end_len = strlen(end);
	size_t	got;
	int	c;

	/* Wait for start string */
	got = 0;
	for (;;) {
		c = lego_getc(l);
		if (c < 0)
			return c;
		buf[got++] = c;
		if (got >= start_len) {
			if (memcmp(buf, start, start_len) == 0)
				break;
			memmove(buf, buf + 1, got - 1);
			got--;
		}
	}

	/* Wait for end string */
	for (;;) {
		c = lego_getc(l);
		if (c < 0)
			return c;
		if (got >= len)
			return LEGO_ERR;
		buf[got++] = c;
		if (got >= end_len) {
			if (memcmp(buf + got - end_len, end, end_len) == 0)
				break;
		}
	}

	if (got >= len)
		return -1;

	buf[got] = '\0';

	return got;
}

static const char init_string[] = "p\0###Do you byte, when I knock?$$$";

static int
lego_init(lego_buf_t *l)
{
	char	buf[LEGO_BUFSIZE];
	int	try;
	int	got;

	for (try = 0; try < 3; try++) {
		lego_lock(l);

		write(l->fd, init_string, sizeof(init_string) - 1);

		lego_unlock(l);

		got = lego_read_until(l, buf, sizeof(buf), "###", "$$$");

		if (got >= 0)
			break;
	}

	if (got < 0)
		return got;

	l->keepalive = milliseconds();
	l->connected = true;
	return LEGO_OK;
}

typedef struct {
	uint8_t	sensors[8][2];

	uint8_t checksum;
} lego_sensor_packet_t;

#define SENSOR_4	0
#define SENSOR_8	1
#define SENSOR_3	2
#define SENSOR_7	3
#define SENSOR_2	4
#define SENSOR_6	5
#define SENSOR_1	6
#define SENSOR_5	7

static const uint8_t	sensor_map[] = {
	SENSOR_1,
	SENSOR_2,
	SENSOR_3,
	SENSOR_4,
	SENSOR_5,
	SENSOR_6,
	SENSOR_7,
	SENSOR_8,
};

/* Extract the ADC and "status" values. */
static lego_sensor_t
lego_parse_sensor(lego_sensor_packet_t *sensor, int i)
{
	int	m = sensor_map[i];
	lego_sensor_t	value = {
		.value = sensor->sensors[m][0] << 2 | sensor->sensors[m][1] >> 6,
		.status = sensor->sensors[m][1] & 0x3f
	};
	return value;
}

/*
 * Compute checksum of sensor packet. Note that the two leading zeros
 * are probably supposed to be included, but as they are zero, they
 * don't affect the result
 */
static uint8_t
lego_chksum_sensor_packet(lego_sensor_packet_t *sensor)
{
	uint8_t	*c = (uint8_t *) sensor;
	size_t	i;
	uint8_t	sum = 0;

	for (i = 0; i < sizeof(lego_sensor_packet_t); i++)
		sum += c[i];
	return sum;
}

/*
 * Read a complete sensor packet
 */
static int
lego_read_sensor_packet(lego_buf_t *l, lego_sensor_packet_t *sensor)
{
	int	c;
	size_t	i;
	uint8_t	*buf;

	/* Find header */
	for (;;) {
		c = lego_getc(l);
		if (c < 0)
			return c;
		if (c != 0)
			continue;
		c = lego_getc(l);
		if (c < 0)
			return c;
		if (c == 0)
			break;
	}
	buf = (uint8_t *) sensor;
	for (i = 0; i < sizeof(lego_sensor_packet_t); i++) {
		c = lego_getc(l);
		if (c < 0)
			return c;
		buf[i] = c;
	}
	if (lego_chksum_sensor_packet(sensor) != 0xff)
		return LEGO_CHKSUM;
	return LEGO_OK;
}

static void *
lego_reader (void *_closure)
{
	lego_buf_t		*l = _closure;
	lego_sensor_packet_t	sensor;
	int			ret;
	int			i;

	while (l->running) {
		if (!l->connected) {
			if (lego_init(l) != LEGO_OK) {
				usleep(1000000);
				continue;
			}
		}

		/* keep alive */
		uint32_t	now = milliseconds();
		if (now - l->keepalive >= KEEPALIVE_MS) {
			char	nop = 2;
			lego_lock(l);
			write(l->fd, &nop, 1);
			lego_unlock(l);
			l->keepalive = now;
		}

		/* read sensor packet */
		ret = lego_read_sensor_packet(l, &sensor);

		/* packet data error, start over */
		if (ret == LEGO_CHKSUM) {
			lego_flush(l);
			continue;
		}
		/* timeout or read failure. re-initialize */
		if (ret < 0) {
			l->connected = false;
			continue;
		}

		lego_lock(l);

		for (i = 0; i < LEGO_SENSORS; i++)
			l->sensors[i] = lego_parse_sensor(&sensor, i);

		lego_unlock(l);
	}
	return NULL;
}

int
lego_start(lego_buf_t *l, char *device)
{
	l->fd = open(device, O_RDWR);
	if (l->fd < 0)
		return LEGO_ERR;

	struct termios termios;
	int ret;

	ret = tcgetattr(l->fd, &termios);
	if (ret < 0)
		return LEGO_ERR;

	cfmakeraw(&termios);
	cfsetspeed(&termios, B9600);

	ret = tcsetattr(l->fd, TCOFLUSH, &termios);
	if (ret < 0)
		return LEGO_ERR;

	l->out = fdopen(l->fd, "w");
	l->count = l->used = 0;

	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init(&l->mutex, &attr);

	if (lego_init(l) != LEGO_OK)
		return LEGO_TIMEOUT;

	l->running = true;

	pthread_create(&l->thread, NULL, lego_reader, l);

	return LEGO_OK;
}

void
lego_stop(lego_buf_t *l)
{
	if (l->running) {
		l->running = false;
		pthread_join(l->thread, NULL);
		pthread_mutex_destroy(&l->mutex);
		fclose(l->out);
		close(l->fd);
	}
}

lego_sensor_t
lego_sensor(lego_buf_t *l, int i)
{
	lego_sensor_t	v;

	lego_lock(l);
	v = l->sensors[i];
	lego_unlock(l);
	return v;
}

bool
lego_connected(lego_buf_t *l)
{
	bool ret;

	lego_lock(l);
	ret = l->connected;
	lego_unlock(l);
	return ret;
}
