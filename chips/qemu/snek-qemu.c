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
#define _HAVE_SEMIHOST
#endif
#ifdef _HAVE_SEMIHOST
#include <semihost.h>
#endif
#include <sys/time.h>

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
	uint32_t mod = snek_poly_get_float(a);

	if (!mod) {
		snek_error_value(a);
		return SNEK_NULL;
	}
	random_x *= random_x;
	random_w += random_s;
	random_x += random_w;
	random_x = (random_x >> 32) | (random_x << 32);
	return snek_float_to_poly(random_x % mod);
}

static uint32_t
centisecs(void)
{
#ifdef _HAVE_SEMIHOST
	return (uint32_t) (sys_semihost_elapsed() / (sys_semihost_tickfreq()/100));
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);

	uint32_t centi = tv.tv_sec * 100 + tv.tv_usec / 10000;
	return centi;
#endif
}

snek_poly_t
snek_builtin_time_monotonic(void)
{
	return snek_float_to_poly((float) centisecs() * 0.01f);
}

snek_poly_t
snek_builtin_time_sleep(snek_poly_t a)
{
	if (snek_poly_type(a) == snek_float) {
		int32_t csecs = floorf(snek_poly_to_float(a) * 100.0f);
		uint32_t then = centisecs() + csecs;

		while ((int32_t) (then - centisecs()) > 0)
			;
	}
	return SNEK_NULL;
}


static FILE *snek_qemu_file;

int snek_qemu_getc(void)
{
	if (snek_qemu_file)
		return getc(snek_qemu_file);
	return snek_io_getc(stdin);
}

int
main(int argc, char **argv)
{
	snek_init();

	char *file = NULL;

	if (argc > 1)
		file = argv[1];

	if (file) {
		snek_file = file;
		snek_qemu_file = fopen(file, "r");
		if  (!snek_qemu_file) {
			printf("\"%s\": cannot open\n", file);
			exit(1);
		}
	} else {
		snek_interactive = true;
		printf("Welcome to snek " SNEK_VERSION "\n");
	}
	int ret = (snek_parse() == snek_parse_success) ? 0 : 1;
	if (snek_interactive)
		printf("\n");
	fflush(stdout);
	exit(ret);
}
