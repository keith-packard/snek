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

#include <snek-io.h>

#define RX_LINEBUF	132
#define SNEK_POOL	(32 * 1024)

extern int snek_qemu_getc(void);

#define abort() exit(1)

#define SNEK_GETC()		snek_qemu_getc()

#define SNEK_IO_GETC(file)	getc(stdin)
