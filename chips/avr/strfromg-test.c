/*
 * Copyright © 2024 Keith Packard <keithp@keithp.com>
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

#define __STDC_WANT_IEC_60559_BFP_EXT__
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "strfromg.h"

static float
int_exp10(int n)
{
        float      	a = 1.0f;
        int             sign = n < 0;

        if (sign)
                n = -n;
        while (n--)
                a *= 10.0f;
        if (sign)
                a = 1/a;
        return a;
}

static const float test_vals[] = { 1.234567f, 1.1f, (float) M_PI };

int
main(int argc, char **argv)
{
	char	buf[16];
	char	libc[16];
	int	x;
	int	i;
	int	ret = 0;

	for (i = 1; i < argc; i++) {
		float	val = strtof(argv[i], NULL);
		strfromg(buf, val);
		strfromf(libc, sizeof(libc), "%.8g", val);
		printf("libc '%s' snek '%s'\n", libc, buf);
	}
	for (x = -45; x <= 38; x++)
	{
                float r;
		unsigned t;
		for (t = 0; t < sizeof(test_vals)/sizeof(test_vals[0]); t++) {
			float v = test_vals[t] * int_exp10(x);
			float e;
			strfromg(buf, v);
			strfromf(libc, sizeof(libc), "%.8g", v);
			if (strcmp(buf, libc)) {
				printf("libc %s snek %s\n", libc, buf);
				ret = 1;
			}
		}
	}
	return ret;
}
