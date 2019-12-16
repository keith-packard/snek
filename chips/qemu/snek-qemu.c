/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2019 Keith Packard
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <snek.h>
#include <snek-io.h>
#ifdef HAVE_SEMIHOST
#include <semihost.h>
#endif

snek_poly_t
snek_builtin_exit(snek_poly_t a)
{
	int ret;
	switch (snek_poly_type(a)) {
	case snek_float:
		ret = (int) snek_poly_to_float(a);
		break;
	default:
		ret = snek_poly_true(a) ? 0 : 1;
		break;
	}
	exit(ret);
	return a;
}

static uint64_t random_x, random_w;

#define random_s 0xb5ad4eceda1ce2a9ULL

snek_poly_t
snek_builtin_random_seed(snek_poly_t a)
{
	random_x = a.u;
	random_x |= random_x << 32;
	random_w = 0;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_random_randrange(snek_poly_t a)
{
	random_x *= random_x;
	random_w += random_s;
	random_x += random_w;
	random_x = (random_x >> 32) | (random_x << 32);
	return snek_float_to_poly((snek_soffset_t) (random_x % snek_poly_get_soffset(a)));
}

snek_poly_t
snek_builtin_time_monotonic(void)
{
#ifdef HAVE_SEMIHOST
	return snek_float_to_poly((float) sys_semihost_clock() / 100.0f);
#else
	static float now;
	now += 0.01;
	return snek_float_to_poly(now);
#endif
}

snek_poly_t
snek_builtin_time_sleep(snek_poly_t a)
{
#ifdef  HAVE_SEMIHOST
	if (snek_poly_type(a) == snek_float) {
		int csecs = floorf(snek_poly_to_float(a) * 100.0f);
		uintptr_t then = sys_semihost_clock() + csecs;

		while (sys_semihost_clock() < then)
			;
	}
#endif
	return SNEK_NULL;
}


static FILE *snek_qemu_file;

int snek_qemu_getc(void)
{
	if (snek_qemu_file)
		return getc(snek_qemu_file);
	return snek_io_getc(stdin);
}

#ifdef HAVE_SEMIHOST
#define CMDLINE_SIZE 128
static char cmdline[CMDLINE_SIZE];
#endif

int
main(void)
{
	snek_init();

#ifdef HAVE_SEMIHOST
	char *file = NULL;

	/* This returns the whole command line, including  argv[0] */
	if (sys_semihost_get_cmdline(cmdline, CMDLINE_SIZE - 1) == 0) {

		/* Skip to the first argument. This assumes there
		 * are no spaces in the command name
		 */
		file = strchr(cmdline, ' ');
		if (file)
			file++;
	}

	if (file) {
		snek_file = file;
		snek_qemu_file = fopen(file, "r");
		if  (!snek_qemu_file) {
			printf("\"%s\": cannot open\n", file);
			exit(1);
		}
	} else
#endif
	{
		snek_interactive = true;
		printf("Welcome to snek " SNEK_VERSION "\n");
	}
	int ret = (snek_parse() == snek_parse_success) ? 0 : 1;
	if (snek_interactive)
		printf("\n");
	fflush(stdout);
	exit(ret);
}
