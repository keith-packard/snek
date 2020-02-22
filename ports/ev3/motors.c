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
#include "motors.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#define OUT_PORTS_N 4

#define EV3_PORTS_OUT_PREFIX "ev3-ports:out"
#define EV3_PORTS_OUT_PREFIX_LEN (sizeof(EV3_PORTS_OUT_PREFIX) - 1)

#define MOTOR_DIR "/sys/class/tacho-motor"

#define MOTOR_PREFIX "motor"
#define MOTOR_PREFIX_LEN (sizeof(MOTOR_PREFIX) - 1)

#define UNIT_PER_ROT 360
#define MIN_SPEED -1050
#define MAX_SPEED 1050

#define DURATION_SCALE 1000

#define CMD_RUN_FOREVER "run-forever"
#define CMD_RUN_FOREVER_LEN (sizeof(CMD_RUN_FOREVER) - 1)

#define CMD_STOP "stop"
#define CMD_STOP_LEN (sizeof(CMD_STOP) - 1)

#define CMD_RUN_TIMED "run-timed"
#define CMD_RUN_TIMED_LEN (sizeof(CMD_RUN_TIMED) - 1)

#define CMD_RUN_TO_ABS_POS "run-to-abs-pos"
#define CMD_RUN_TO_ABS_POS_LEN (sizeof(CMD_RUN_TO_ABS_POS) - 1)

#define POLARITY_NORMAL "normal"
#define POLARITY_NORMAL_LEN (sizeof(POLARITY_NORMAL) - 1)

#define POLARITY_INVERSED "inversed"
#define POLARITY_INVERSED_LEN (sizeof(POLARITY_INVERSED) - 1)

#define STOP_COAST "coast"
#define STOP_COAST_LEN (sizeof(STOP_COAST) - 1)

#define STOP_BRAKE "brake"
#define STOP_BRAKE_LEN (sizeof(STOP_BRAKE) - 1)

#define STOP_HOLD "hold"
#define STOP_HOLD_LEN (sizeof(STOP_HOLD) - 1)

/* global data */

static int motors[OUT_PORTS_N];
static int selected_motor = -1;

/* end of global data */

/*
 * All functions in this file error call snek_error and return -1 or SNEK_NULL
 * on error, except where documented otherwise
 */

#define ADDRESS_LEN 64

static int
get_selected_motor(void)
{
	if (selected_motor == -1) {
		snek_error("no selected motor");
		return -1;
	}
	return selected_motor;
}

static int
discover_motor2(int fd, int num)
{
	char address[ADDRESS_LEN] = "";
	if (sysfs_read(fd, "address", address, ADDRESS_LEN - 1) == -1) {
		snek_error("unable to find a port of motor: %s", strerror(errno));
		return -1;
	}

	const char *portsuffix = cutprefix(address, EV3_PORTS_OUT_PREFIX, EV3_PORTS_OUT_PREFIX_LEN);
	if (portsuffix == NULL) {
		snek_error("unexpected port name: %s", address);
		return -1;
	}

	if (portsuffix[0] < 'A' || portsuffix[0] > 'D' || portsuffix[1] != '\n') {
		snek_error("unexpected port name, Lego EV3 only has output ports A..D, not %s", address);
		return -1;
	}

	motors[portsuffix[0] - 'A'] = num;
	return 0;
}

static int
discover_motor(int motors_fd, const char *filename, int num)
{
	int sd = openat(motors_fd, filename, O_RDONLY | O_DIRECTORY);
	if (sd == -1) {
		/* ENOENT here means the motor was disconnected during directory enumeration. */
		if (errno == ENOENT)
			return 0;
		snek_error("unable to open motor directory: %s", strerror(errno));
		return -1;
	}

	int ret = discover_motor2(sd, num);

	close(sd);
	return ret;
}

static int
discover_motors2(int motors_fd, DIR *motors_dir)
{
	for (struct dirent *ent = readdir(motors_dir); ent; ent = readdir(motors_dir)) {
		if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
			continue;
		}

		const char *suffix = cutprefix(ent->d_name, MOTOR_PREFIX, MOTOR_PREFIX_LEN);
		if (suffix == NULL) {
			snek_error("unexpected filename in motors directory: %s", ent->d_name);
			return -1;
		}

		int num;
		if (sscanf(suffix, "%d", &num) != 1) {
			snek_error("unexpected filename in motors directory: %s", ent->d_name);
			return -1;
		}

		if (discover_motor(motors_fd, ent->d_name, num) == -1)
			return -1;
	}

	return 0;
}

static int
discover_motors(void)
{
	memset(motors, 0, sizeof(motors));

	int fd = open(MOTOR_DIR, O_RDONLY | O_DIRECTORY);
	if (fd == -1) {
		snek_error("unable to open motors directory: %s", strerror(errno));
		return -1;
	}

	DIR *dir = fdopendir(fd);
	if (dir == NULL) {
		close(fd);
		snek_error("unable to enumerate motors: %s", strerror(errno));
		return -1;
	}

	int ret = discover_motors2(fd, dir);

	closedir(dir);
	return ret;
}

#define MOTOR_PATH_BUF_LEN 256

/* Returns -1 and sets errno on error, does not call snek_error */
static int
open_motor(int p)
{
	char buf[MOTOR_PATH_BUF_LEN];
	sprintf(buf, MOTOR_DIR "/" MOTOR_PREFIX "%d", motors[p]);
	return open(buf, O_RDONLY | O_DIRECTORY);
}

static int
get_motori(int port)
{
	int fd = open_motor(port);
	if (fd == -1) {
		if (discover_motors() == -1)
			return -1;
		fd = open_motor(port);
	}

	if (fd == -1) {
		if (errno == ENOENT)
			snek_error("no motor attached to port %c", port + 'A');
		else
			snek_error("unable to open motor attached to port %c :%s", port + 'A', strerror(errno));
	}
	return fd;
}

static int
get_motor_port(snek_poly_t port)
{
	char *c = snek_poly_to_string(port);
	if (c[0] < 'A' || c[0] > 'D' || c[1] != '\0') {
		snek_error("Lego EV3 has only output ports A..D, not %s", c);
		return -1;
	}

	return c[0] - 'A';
}

static int
get_motor(snek_poly_t port)
{
	int iport = get_motor_port(port);
	if (iport == -1)
		return -1;

	return get_motori(iport);
}

static snek_poly_t
motor_speed(int fd)
{
	int val;
	if (sysfs_read_int(fd, "speed", &val) == -1) {
		snek_error("failed to read speed from motor: %s", strerror(errno));
		return SNEK_NULL;
	}

	/* TODO: hardcoded scale. May need to be parameterized for new motors */
	return snek_float_to_poly(((float) val) / UNIT_PER_ROT);
}

snek_poly_t
snek_ev3_motors_read(snek_poly_t port)
{
	int fd = get_motor(port);
	if (fd == -1)
		return SNEK_NULL;

	snek_poly_t ret = motor_speed(fd);

	close(fd);
	return ret;
}

static int
driver_speed(float speed)
{
	int s = speed * UNIT_PER_ROT;
	if (s < MIN_SPEED)
		s = MIN_SPEED;
	if (s > MAX_SPEED)
		s = MAX_SPEED;
	return s;
}

snek_poly_t
snek_builtin_talkto(snek_poly_t port)
{
	int iport = get_motor_port(port);
	if (iport == -1)
		return SNEK_NULL;

	selected_motor = iport;
	return SNEK_NULL;
}

typedef snek_poly_t (*motor_fn0)(int fd);

static snek_poly_t
call_with_current_motor0(motor_fn0 fn)
{
	int port = get_selected_motor();
	if (port == -1)
		return SNEK_NULL;

	int fd = get_motori(port);
	if (fd == -1)
		return SNEK_NULL;

	snek_poly_t ret = fn(fd);

	close(fd);
	return ret;
}

typedef snek_poly_t (*motor_fn1)(int fd, snek_poly_t arg);

static snek_poly_t
call_with_current_motor1(motor_fn1 fn, snek_poly_t arg)
{
	int port = get_selected_motor();
	if (port == -1)
		return SNEK_NULL;

	int fd = get_motori(port);
	if (fd == -1)
		return SNEK_NULL;

	snek_poly_t ret = fn(fd, arg);

	close(fd);
	return ret;
}

static snek_poly_t
motor_setspeed(int fd, snek_poly_t speed)
{
	if (snek_poly_type(speed) != snek_float) {
		snek_error_type_1(speed);
		return SNEK_NULL;
	}

	float fspeed = snek_poly_get_float(speed);

	if (sysfs_write_int(fd, "speed_sp", driver_speed(fspeed)) == -1) {
		snek_error("unable to set speed: %s", strerror(errno));
	}

	return SNEK_NULL;
}

snek_poly_t
snek_builtin_setpower(snek_poly_t power)
{
	return call_with_current_motor1(motor_setspeed, power);
}

static snek_poly_t
motor_setleft(int fd)
{
	if (sysfs_write(fd, "polarity", POLARITY_INVERSED, POLARITY_INVERSED_LEN) == -1) {
		snek_error("unable to set motor to rotate counterclockwise: %s", strerror(errno));
	}
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_setleft(void)
{
	return call_with_current_motor0(motor_setleft);
}

static snek_poly_t
motor_setright(int fd)
{
	if (sysfs_write(fd, "polarity", POLARITY_NORMAL, POLARITY_NORMAL_LEN) == -1) {
		snek_error("unable to set motor to rotate clockwise: %s", strerror(errno));
	}
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_setright(void)
{
	return call_with_current_motor0(motor_setright);
	return SNEK_NULL;
}

static snek_poly_t
motor_on(int fd)
{
	if (sysfs_write(fd, "command", CMD_RUN_FOREVER, CMD_RUN_FOREVER_LEN) == -1) {
		snek_error("unable to start motor: %s", strerror(errno));
	}
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_on(void)
{
	return call_with_current_motor0(motor_on);
}

static snek_poly_t
motor_off(int fd)
{
	if (sysfs_write(fd, "command", CMD_STOP, CMD_STOP_LEN) == -1) {
		snek_error("unable to stop motor: %s", strerror(errno));
	}
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_off(void)
{
	return call_with_current_motor0(motor_off);
}

static snek_poly_t
motor_onfor(int fd, snek_poly_t duration)
{
	if (snek_poly_type(duration) != snek_float) {
		snek_error_type_1(duration);
		return SNEK_NULL;
	}

	float fduration = snek_poly_get_float(duration);

	if (sysfs_write_int(fd, "time_sp", ((int) fduration * DURATION_SCALE)) == -1) {
		snek_error("unable to set run duration: %s", strerror(errno));
		return SNEK_NULL;
	}

	if (sysfs_write(fd, "command", CMD_RUN_TIMED, CMD_RUN_TIMED_LEN) == -1) {
		snek_error("unable to start motor: %s", strerror(errno));
	}

	return SNEK_NULL;
}

snek_poly_t
snek_builtin_onfor(snek_poly_t duration)
{
	return call_with_current_motor1(motor_onfor, duration);
}

static snek_poly_t
motor_setposition(int fd, snek_poly_t position)
{
	if (snek_poly_type(position) != snek_float) {
		snek_error_type_1(position);
		return SNEK_NULL;
	}

	int iposition = snek_poly_get_float(position) * UNIT_PER_ROT;

	if (sysfs_write_int(fd, "position_sp", iposition) == -1) {
		snek_error("unable to set target position: %s", strerror(errno));
		return SNEK_NULL;
	}

	if (sysfs_write(fd, "command", CMD_RUN_TO_ABS_POS, CMD_RUN_TO_ABS_POS_LEN) == -1) {
		snek_error("unable to start motor: %s", strerror(errno));
	}

	return SNEK_NULL;
}

snek_poly_t
snek_builtin_setposition(snek_poly_t position)
{
	return call_with_current_motor1(motor_setposition, position);
}

static snek_poly_t
motor_position(int fd)
{
	int val;
	if (sysfs_read_int(fd, "position", &val) == -1) {
		snek_error("failed to read speed position from motor: %s", strerror(errno));
		return SNEK_NULL;
	}

	/* TODO: hardcoded divisor. use "count_per_rot" file */
	return snek_float_to_poly(((float) val) / UNIT_PER_ROT);
}

snek_poly_t
snek_builtin_position(void)
{
	return call_with_current_motor0(motor_position);
}

static snek_poly_t
motor_coast(int fd)
{
	if (sysfs_write(fd, "stop_action", STOP_COAST, STOP_COAST_LEN) == -1) {
		snek_error("unable to set stop action: %s", strerror(errno));
	}
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_coast(void)
{
	return call_with_current_motor0(motor_coast);
}

static snek_poly_t
motor_brake(int fd)
{
	if (sysfs_write(fd, "stop_action", STOP_BRAKE, STOP_BRAKE_LEN) == -1) {
		snek_error("unable to set stop action: %s", strerror(errno));
	}
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_brake(void)
{
	return call_with_current_motor0(motor_brake);
}

static snek_poly_t
motor_hold(int fd)
{
	if (sysfs_write(fd, "stop_action", STOP_HOLD, STOP_HOLD_LEN) == -1) {
		snek_error("unable to set stop action: %s", strerror(errno));
	}
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_hold(void)
{
	return call_with_current_motor0(motor_hold);
}

static snek_poly_t
motor_ramp(int fd, const char *file, snek_poly_t time, const char *name)
{
	if (snek_poly_type(time) != snek_float) {
		snek_error_type_1(time);
		return SNEK_NULL;
	}

	/* driver time is in milliseconds */
	int driver_time = (int) (1000 * snek_poly_get_float(time));

	if (sysfs_write_int(fd, file, driver_time) == -1) {
		snek_error("unable to set %s time: %s", name, strerror(errno));
	}
	return SNEK_NULL;
}

static snek_poly_t
motor_ramp_up(int fd, snek_poly_t time)
{
	return motor_ramp(fd, "ramp_up_sp", time, "ramp-up");
}

snek_poly_t
snek_builtin_ramp_up(snek_poly_t time)
{
	return call_with_current_motor1(motor_ramp_up, time);
}

static snek_poly_t
motor_ramp_down(int fd, snek_poly_t time)
{
	return motor_ramp(fd, "ramp_down_sp", time, "ramp-down");
}

snek_poly_t
snek_builtin_ramp_down(snek_poly_t time)
{
	return call_with_current_motor1(motor_ramp_down, time);
}
