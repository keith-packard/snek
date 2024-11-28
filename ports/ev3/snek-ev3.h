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
#ifndef _SNEK_EV3_H_
#define _SNEK_EV3_H_

/*
 * Stable ev3dev version is based on Debian stretch, with glibc 2.24.
 *
 * glibc 2.27 has changed[1] the version of several libm symbols due to dropped
 * error handling compatibility with System V release 4, so binaries linked
 * against glibc >= 2.27 (e.g. in Debian buster and later) fail to load on
 * ev3dev with
 *
 *   /lib/arm-linux-gnueabi/libm.so.6:
 *   version `GLIBC_2.27' not found (required by ./snek-ev3-1.3)
 *
 * Force using older versions of these symbols to allow linking against newer
 * glibc and hence simplify build setup.
 *
 * [1] https://sourceware.org/git/?p=glibc.git;a=commit;h=f5f0f5265162fe6f4f238abcd3086985f7c38d6d
 */
#if defined(SNEK_USE_GLIBC_2_4_MATH)
__asm__(".symver expf,expf@GLIBC_2.4");
__asm__(".symver exp2f,exp2f@GLIBC_2.4");
__asm__(".symver logf,logf@GLIBC_2.4");
__asm__(".symver log2f,log2f@GLIBC_2.4");
__asm__(".symver powf,powf@GLIBC_2.4");
#endif

int
snek_getc(void);

#define SNEK_GETC() snek_getc()
#define SNEK_IO_GETC(file) getc(file)

#define CONST const

#endif /* _SNEK_EV3_H_ */
