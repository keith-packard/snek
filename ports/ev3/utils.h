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
#ifndef _SNEK_EV3_UTILS_H_
#define _SNEK_EV3_UTILS_H_

#include <sys/types.h>
#include <dirent.h>

/*
 * Returns pointer to part of string after prefix if s starts with prefixlen
 * bytes of prefix, NULL otherwise.
 */
const char *
cutprefix(const char *s, const char *prefix, int prefixlen);

/*
 * read(2) that retries in case of EINTR
 */
ssize_t
read_noeintr(int fd, char *buf, size_t bufsize);

/*
 * write(2) that retries in case of EINTR
 */
ssize_t
write_noeintr(int fd, const char *buf, size_t bufsize);

/*
 * close(2) that retries in case of EINTR
 */
int
close_noeintr(int fd);

/*
 * Reads up to bufsize bytes of data from sysfs file filename in directory dirfd
 * and places it into bufffer buf.
 *
 * Returns number of bytes read on success, -1 and sets errno on error.
 */
ssize_t
sysfs_read(int dirfd, const char *filename, char *buf, size_t bufsize);

/*
 * Reads an integer from a sysfs file filename in directory dirfd and places it
 * into val.
 *
 * Returns 0 on success, -1 and sets errno on error.
 */
int
sysfs_read_int(int dirfd, const char *filename, int *val);

/*
 * Reads an integer from a sysfs file filename in directory dirfd and places it
 * into val, if it is in [min, max] range, inclusive.
 *
 * Returns 0 on success, -1 and sets errno on error.
 */
int
sysfs_read_int_range(int dirfd, const char *filename, int *val, int min, int max);

/*
 * Writes bufsize bytes of data from buf to sysfs file filename in directory
 * dirfd.
 *
 * Returns 0 on success, -1 and sets errno on error.
 */
int
sysfs_write(int dirfd, const char *filename, const char *buf, size_t bufsize);

/*
 * Writes integer val to sysfs file filename in directory dirfd.
 *
 * Returns 0 on success, -1 and sets errno on error.
 */
int
sysfs_write_int(int dirfd, const char *filename, int val);

#endif /* _SNEK_EV3_UTILS_H_ */
