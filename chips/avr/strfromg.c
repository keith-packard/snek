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

#ifndef STRFROMG_TEST
#include "snek.h"
#else
#include <string.h>
#define snek_const_strcpy(a,b) strcpy(a,b)
#endif

#include "strfromg.h"
#include <stdint.h>

int __ftoa_engine (float val, char *buf,
                   unsigned char prec, unsigned char maxdgs);

/* '__ftoa_engine' return next flags (in buf[0]):	*/
#define	FTOA_MINUS	1
#define	FTOA_ZERO	2
#define	FTOA_INF	4
#define	FTOA_NAN	8
#define	FTOA_CARRY	16	/* Carry was to master position.	*/

#define PREC		7

int
strfromg(char *dst0, float val)
{
	char	*dst = dst0;
	char	buf[1 + 1 + PREC + 1];
	int	exp;
	uint8_t	vtype;
	uint8_t	prec = PREC;
	uint8_t	fix = 0;
	int	ndigs = 0;
	int	n;
	char 	c;

	exp = __ftoa_engine(val, buf, prec, 0);
	vtype = buf[0];
	if (vtype & FTOA_MINUS)
		*dst++ = '-';
	if (vtype & FTOA_NAN) {
		snek_const_strcpy(dst, "nan");
		dst += 3;
	} else if (vtype & FTOA_INF) {
		snek_const_strcpy(dst, "inf");
		dst += 3;
	} else {
		if (-4 <= exp && exp <= prec)
			fix = 1;
		while (prec && buf[1+prec] == '0')
			prec--;
		if (fix) {	/* 'f' format */
			ndigs = prec + 1;
			if (prec > exp)
				prec = prec - exp;
			else
				prec = 0;
			n = exp > 0 ? exp : 0;
			for (;;) {
				if (n == -1)
					*dst++ = '.';
				c = (n <= exp && n > exp - ndigs) ? buf[exp - n + 1] : '0';
				if (--n < -prec)
					break;
				*dst++ = c;
			}
			if (n == exp &&
			    (buf[1] > '5'
			     || (buf[1] == '5' && !(vtype & FTOA_CARRY))))
				c = '1';
			*dst++ = c;
		} else {	/* 'g' format */
			if (buf[1] != '1')
				vtype &= ~FTOA_CARRY;
			*dst++ = buf[1];
			if (prec) {
				uint8_t dig = 2;
				*dst++ = '.';
				do {
					*dst++ = buf[dig++];
				} while (--prec);
			}

			/* exponent */
			*dst++ = 'e';
			c = '+';
			if (exp < 0 || (exp == 0 && (vtype & FTOA_CARRY))) {
				exp = -exp;
				c = '-';
			}
			*dst++ = c;
			*dst++ = (exp / 10) + '0';
			*dst++ = (exp % 10) + '0';
		}

		*dst = '\0';
	}
	return dst - dst0;
}
