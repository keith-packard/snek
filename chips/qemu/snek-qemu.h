/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
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

#pragma once

#define SNEK_GETC()		snek_qemu_getc()

#define SNEK_IO_GETC(file)	getc(stdin)

#include <snek-io.h>

#define RX_LINEBUF	132
#define SNEK_POOL	(32 * 1024)

extern int snek_qemu_getc(void);

#define abort() exit(1)

#define CONST const

#if _PICOLIBC__ == 1 && (_PICOLIBC_MINOR__ < 6 || __PICOLIBC_PATCHLEVEL__ < 1)
#define _SNEK_NEED_STRFROMF
int
strfromf(char *__restrict str, size_t n,
	 const char *__restrict format, float fp);
#endif

#if _PICOLIBC__ == 1 && (_PICOLIBC_MINOR__ < 7 || __PICOLIBC_PATCHLEVEL__ < 5)
#define _SNEK_NEED_STRTOF
#endif

