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
#include "utils.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

const char *
cutprefix(const char *s, const char *prefix, int prefixlen)
{
	if (memcmp(s, prefix, prefixlen) != 0)
		return NULL;
	return s + prefixlen;
}

static void
close_noerrno(int fd)
{
	int saved_errno = errno;
	close(fd);
	errno = saved_errno;
}

ssize_t
read_noeintr(int fd, char *buf, size_t bufsize)
{
	for (;;) {
		int read_ = read(fd, buf, bufsize);
		if (read_ == -1 && errno == EINTR)
			continue;
		return read_;
	}
}

ssize_t
write_noeintr(int fd, const char *buf, size_t bufsize)
{
	for (;;) {
		int written = write(fd, buf, bufsize);
		if (written == -1 && errno == EINTR)
			continue;
		return written;
	}
}

int
close_noeintr(int fd)
{
	for (;;) {
		int ret = close(fd);
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

ssize_t
sysfs_read(int dirfd, const char *filename, char *buf, size_t bufsize)
{
	int fd = openat(dirfd, filename, O_RDONLY);
	if (fd == -1) {
		return -1;
	}

	int read_ = read_noeintr(fd, buf, bufsize);
	close_noerrno(fd);
	return read_;
}

// len(maxint) + newline + \0
#define INTBUFSIZE 12

int
sysfs_read_int(int dirfd, const char *filename, int *val)
{
	char intbuf[INTBUFSIZE] = {};

	int read_ = sysfs_read(dirfd, filename, intbuf, INTBUFSIZE - 1);
	if (read_ == -1)
		return -1;

	if (sscanf(intbuf, "%d", val) != 1) {
		errno = EINVAL;
		return -1;
	}

	return 0;
}

int
sysfs_read_int_range(int dirfd, const char *filename, int *val, int min, int max)
{
	if (sysfs_read_int(dirfd, filename, val) == -1)
		return -1;

	if (*val < min || *val > max) {
		errno = ERANGE;
		return -1;
	}

	return 0;
}

int
sysfs_write(int dirfd, const char *filename, const char *buf, size_t bufsize)
{
	int fd = openat(dirfd, filename, O_WRONLY);
	if (fd == -1) {
		return -1;
	}

	ssize_t written = write_noeintr(fd, buf, bufsize);
	close_noerrno(fd);

	if (written == -1)
		return -1;
	if (written != (ssize_t) bufsize) {
		errno = EIO;
		return -1;
	}
	return 0;
}

int
sysfs_write_int(int dirfd, const char *filename, int val)
{
	char buf[INTBUFSIZE] = "";
	int  len = sprintf(buf, "%d", val);

	return sysfs_write(dirfd, filename, buf, len);
}
