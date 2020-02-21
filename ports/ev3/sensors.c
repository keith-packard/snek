/*
 * Copyright © 2020 Mikhail Gusarov <dottedmag@dottedmag.net>
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
 */
#include "snek.h"
#include "utils.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

#define IN_PORTS_N 4

#define EV3_PORTS_IN_PREFIX "ev3-ports:in"
#define EV3_PORTS_IN_PREFIX_LEN (sizeof(EV3_PORTS_IN_PREFIX) - 1)

#define SENSOR_DIR "/sys/class/lego-sensor"
#define SENSOR_DIR_LEN (sizeof(SENSOR_DIR) - 1)

#define SENSOR_PREFIX "sensor"
#define SENSOR_PREFIX_LEN (sizeof(SENSOR_PREFIX) - 1)

#define SENSOR_NONE 0
#define SENSOR_BUTTON 1
#define SENSOR_ANGULAR 2
#define SENSOR_DISTANCE 3
#define SENSOR_LIGHT 4

static const char *KIND_NAMES[] = {
	"---", "button", "angular", "distance", "light",
};

static const char *DRIVER_NAMES[] = {
	"", "lego-ev3-touch\n", "lego-ev3-gyro\n", "lego-ev3-us\n", "lego-ev3-color\n",
};

#define NUM_KINDS (sizeof(DRIVER_NAMES) / sizeof(DRIVER_NAMES[0]))

/* distance sensor */

#define DISTANCE_MAX 2550
#define DISTANCE_SCALE 10

/* light sensor */

#define INTENSITY_MAX 100

static const char *COLORS[] = {
	NULL, "black", "blue", "green", "yellow", "red", "white", "brown",
};

#define COLORS_NUM (sizeof(COLORS) / sizeof(COLORS[0]))

#define RGB_MAX 1020

/* global data */

/*
 * Once sensor is connected to a port, its "number" is incremented. Hence as
 * long as "number" is valid, the kind the sensor stays the same and does not
 * need to be revalidated.
 */
static int sensor_kind[IN_PORTS_N + 1];
static int sensor_num[IN_PORTS_N + 1];

/* end of global data */

/*
 * All functions in this file error call snek_error and return -1 or SNEK_NULL
 * on error, except where documented otherwise
 */

#define ADDRESS_LEN 64

static int
get_sensor_port(int fd)
{
	char address[ADDRESS_LEN] = "";
	if (sysfs_read(fd, "address", address, ADDRESS_LEN - 1) == -1) {
		snek_error("unable to find a port of sensor: %s", strerror(errno));
		return -1;
	}

	const char *suffix = cutprefix(address, EV3_PORTS_IN_PREFIX, EV3_PORTS_IN_PREFIX_LEN);
	if (suffix == NULL) {
		snek_error("unexpected port name: %s", address);
		return -1;
	}

	if (suffix[0] < '1' || suffix[0] >= '1' + IN_PORTS_N || suffix[1] != '\n') {
		snek_error("unexpected port name, Lego EV3 only has ports 1..%d, not %s", IN_PORTS_N, suffix);
		return -1;
	}

	return suffix[0] - '0';
}

#define DRIVER_NAME_LEN 32

static int
get_sensor_kind(int fd)
{
	char driver_name[DRIVER_NAME_LEN] = "";
	if (sysfs_read(fd, "driver_name", driver_name, DRIVER_NAME_LEN - 1) == -1) {
		snek_error("unable to read sensor kind for sensor: %s", strerror(errno));
		return -1;
	}

	for (int i = 1; i < (int) NUM_KINDS; i++) {
		if (!strcmp(driver_name, DRIVER_NAMES[i]))
			return i;
	}

	snek_error("unknown sensor kind %s", driver_name);
	return -1;
}

static int
discover_sensor2(int fd, int num)
{
	int kind = get_sensor_kind(fd);
	if (kind == -1)
		return -1;

	int port = get_sensor_port(fd);
	if (port == -1)
		return -1;

	sensor_kind[port] = kind;
	sensor_num[port] = num;
	return 0;
}

static int
discover_sensor(int sensors_fd, const char *filename, int num)
{
	int sd = openat(sensors_fd, filename, O_RDONLY | O_DIRECTORY);
	if (sd == -1) {
		/* ENOENT here means the sensor was disconnected during
		 * directory enumeration. */
		if (errno == ENOENT)
			return 0;
		snek_error("unable to open sensor directory: %s", strerror(errno));
		return -1;
	}

	int ret = discover_sensor2(sd, num);

	close(sd);
	return ret;
}

static int
discover_sensors2(int sensors_fd, DIR *sensors_dir)
{
	for (struct dirent *ent = readdir(sensors_dir); ent; ent = readdir(sensors_dir)) {
		if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
			continue;
		}

		const char *suffix = cutprefix(ent->d_name, SENSOR_PREFIX, SENSOR_PREFIX_LEN);
		if (suffix == NULL) {
			snek_error("unexpected filename in sensors directory: %s", ent->d_name);
			return -1;
		}

		int num;
		if (sscanf(suffix, "%d", &num) != 1) {
			snek_error("unexpected filename in sensors directory: %s", ent->d_name);
			return -1;
		}

		if (discover_sensor(sensors_fd, ent->d_name, num) == -1)
			return -1;
	}

	return 0;
}

static int
discover_sensors(void)
{
	memset(sensor_kind, 0, sizeof(sensor_kind));
	memset(sensor_num, 0, sizeof(sensor_num));

	int fd = open(SENSOR_DIR, O_RDONLY | O_DIRECTORY);
	if (fd == -1) {
		snek_error("unable to open sensors directory: %s", strerror(errno));
		return -1;
	}

	DIR *dir = fdopendir(fd);
	if (dir == NULL) {
		close(fd);
		snek_error("unable to enumerate sensors: %s", strerror(errno));
		return -1;
	}

	int ret = discover_sensors2(fd, dir);

	closedir(dir);
	return ret;
}

#define SENSOR_PATH_BUF_LEN 256

/* Returns -1 and sets errno on error, does not call snek_error */
static int
open_sensor(int p)
{
	if (sensor_kind[p] == SENSOR_NONE) {
		errno = ENOENT;
		return -1;
	}

	char buf[SENSOR_PATH_BUF_LEN];
	sprintf(buf, SENSOR_DIR "/" SENSOR_PREFIX "%d", sensor_num[p]);

	return open(buf, O_RDONLY | O_DIRECTORY);
}

static int
get_sensor(snek_poly_t port)
{
	snek_soffset_t p = snek_poly_get_soffset(port);

	if (snek_abort)
		return -1;

	if (p <= 0 || p > IN_PORTS_N) {
		snek_error("Lego EV3 has only ports 1..%d, not %d", IN_PORTS_N, p);
		return -1;
	}

	int fd = open_sensor(p);
	if (fd == -1) {
		if (discover_sensors() == -1)
			return -1;
		fd = open_sensor(p);
	}

	if (fd == -1) {
		if (errno == ENOENT)
			snek_error("no sensor attached to port %d", p);
		else
			snek_error("unable to open sensor attached to port %d: %s", p, strerror(errno));
	}

	return fd;
}

static snek_poly_t
button(int fd)
{
	int val;
	if (sysfs_read_int_range(fd, "value0", &val, 0, 1) == -1) {
		snek_error("failed to read from sensor: %s", strerror(errno));
		return SNEK_NULL;
	}

	return val ? SNEK_ONE : SNEK_ZERO;
}

static snek_poly_t
distance(int fd)
{
	int d;
	if (sysfs_read_int(fd, "value0", &d) == -1) {
		snek_error("failed to read from sensor: %s", strerror(errno));
		return SNEK_NULL;
	}

	if (d == DISTANCE_MAX)
		return snek_float_to_poly(INFINITY);

	/* TODO: hardcoded scale. May need to be parameterized for new sensors. */
	return snek_float_to_poly(((float) d) / DISTANCE_SCALE);
}

/* does not have a return value, but sets snek_error on error anyway */
static void
light_set_mode2(int fd, const char *mode)
{
	int kind = get_sensor_kind(fd);
	if (kind == -1) {
		return;
	}

	if (kind != SENSOR_LIGHT) {
		snek_error("expected light sensor, got %s", KIND_NAMES[kind]);
		return;
	}

	if (sysfs_write(fd, "mode", mode, strlen(mode)) == -1) {
		snek_error("unable to set light sensor mode: %s", strerror(errno));
	}
}

static snek_poly_t
light_set_mode(snek_poly_t port, const char *mode)
{
	int fd = get_sensor(port);
	if (fd == -1) {
		return SNEK_NULL;
	}

	light_set_mode2(fd, mode);

	close(fd);
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_light_reflected(snek_poly_t port)
{
	return light_set_mode(port, "COL-REFLECT");
}

snek_poly_t
snek_builtin_light_ambient(snek_poly_t port)
{
	return light_set_mode(port, "COL-AMBIENT");
}

snek_poly_t
snek_builtin_light_color(snek_poly_t port)
{
	return light_set_mode(port, "COL-COLOR");
}

snek_poly_t
snek_builtin_light_rgb(snek_poly_t port)
{
	return light_set_mode(port, "RGB-RAW");
}

static snek_poly_t
light_read_intensity(int fd)
{
	int val;
	if (sysfs_read_int_range(fd, "value0", &val, 0, INTENSITY_MAX) == -1) {
		snek_error("unable to read value");
		return SNEK_NULL;
	}

	/* TODO: hardcoded scale. May need to be parameterized for new sensors */
	return snek_float_to_poly(((float) val) / INTENSITY_MAX);
}

/* TODO: avoid allocations if possible */
static snek_poly_t
sm(const char *s)
{
	size_t len = strlen(s);

	char *new = snek_alloc(len + 1);
	if (new == NULL)
		return SNEK_NULL;

	memcpy(new, s, len + 1);
	return snek_string_to_poly(new);
}

static snek_poly_t
light_read_color_name(int fd)
{
	int val;
	if (sysfs_read_int_range(fd, "value0", &val, 0, COLORS_NUM - 1) == -1) {
		snek_error("unable to read value");
		return SNEK_NULL;
	}

	if (val == 0)
		return SNEK_NULL;

	return sm(COLORS[val]);
}

static snek_poly_t
light_read_rgb(int fd)
{
	int rval, gval, bval;
	if (sysfs_read_int_range(fd, "value0", &rval, 0, RGB_MAX) == -1) {
		snek_error("unable to read RGB value from sensor: %s", strerror(errno));
		return SNEK_NULL;
	}
	if (sysfs_read_int_range(fd, "value1", &gval, 0, RGB_MAX) == -1) {
		snek_error("unable to read RGB value from sensor: %s", strerror(errno));
		return SNEK_NULL;
	}
	if (sysfs_read_int_range(fd, "value2", &bval, 0, RGB_MAX) == -1) {
		snek_error("unable to read RGB value from sensor: %s", strerror(errno));
		return SNEK_NULL;
	}

	snek_stack_push(snek_float_to_poly(((float) rval) / RGB_MAX));
	if (snek_abort)
		return SNEK_NULL;

	snek_stack_push(snek_float_to_poly(((float) gval) / RGB_MAX));
	if (snek_abort)
		return SNEK_NULL;

	snek_stack_push(snek_float_to_poly(((float) bval) / RGB_MAX));
	if (snek_abort)
		return SNEK_NULL;

	return snek_list_imm(3, snek_list_tuple);
}

#define MODE_LEN 16

static snek_poly_t
light(int fd)
{
	char mode[MODE_LEN] = "";
	if (sysfs_read(fd, "mode", mode, MODE_LEN - 1) == -1) {
		snek_error("unable to detect mode of light sensor: %s", strerror(errno));
		return SNEK_NULL;
	}

	snek_poly_t val;
	if (!strcmp(mode, "COL-REFLECT\n") || !strcmp(mode, "COL-AMBIENT\n"))
		val = light_read_intensity(fd);
	else if (!strcmp(mode, "COL-COLOR\n"))
		val = light_read_color_name(fd);
	else if (!strcmp(mode, "RGB-RAW\n"))
		val = light_read_rgb(fd);
	else {
		snek_error("unexpected mode of light sensor: %s", mode);
		val = SNEK_NULL;
	}

	return val;
}

snek_poly_t
snek_builtin_read(snek_poly_t port)
{
	/* TODO: dispatch read() to actuators if a string is passed */
	int fd = get_sensor(port);
	if (fd == -1)
		return SNEK_NULL;

	snek_poly_t val;
	switch (get_sensor_kind(fd)) {
	case SENSOR_BUTTON:
		val = button(fd);
		break;
	case SENSOR_ANGULAR:
		snek_error("angular sensor is not supported yet");
		val = SNEK_NULL;
		break;
	case SENSOR_DISTANCE:
		val = distance(fd);
		break;
	case SENSOR_LIGHT:
		val = light(fd);
		break;
	default:
		/* error is set by get_sensor_kind */
		val = SNEK_NULL;
	}

	close(fd);
	return val;
}
