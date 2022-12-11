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
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "snek.h"
#include <math.h>

#ifdef __AVR__
#define float double
#endif

/*
 * Compute expf using the traditional power series
 *
 * expf(a) = 1 + a/1 + a²/2! + a³/3! …
 */

float
expf(float a)
{
	if (a < 0.0f)
		return 1/expf(-a);

	uint8_t e = 0;

	/*
	 * Values larger than 0.5 need to be scaled so that the series
	 * converges in a reasonable amount of time We scale them to
	 * between 0.25 and 0.5, which makes the series converge in
	 * no more than 8 iterations
	 */
	if (a >= 0.5) {
		int _e;
		a = frexpf(a, &_e)/2.0;
		e = _e + 1;
		if (e > 7)
			return INFINITY;
	}

	float term = 1.0f;
	float sum = 0.0f;
	uint8_t loop;

	/*
	 * It would be better to perform the addition starting with
	 * the smallest terms, as that captures the contributions of
	 * those values better. But, that takes extra code and stack
	 * space.
	 *
	 * This loop converges after 8 iterations in the worst case,
	 * which is when a is the largest value less than 0.5. Instead
	 * of putting conditions in the loop, just run the loop for
	 * 9 iterations each time.
	 */
	for (loop = 1; loop < 9; loop++) {

		/* Compute next term in the sum */
		term *= a / (float) loop;

		/* Add it in */
		sum += term;
	}

	/* sum = sum ** (2 ** e) */
	sum += 1.0f;
	while (e--)
		sum *= sum;
	return sum;
}

/*
 * newtons method computation of log
 *
 *	a = exp(x)
 *	f(x) = exp(x) - a
 *	f'(x) = exp(x)
 *
 *	x' = x - (exp(x) - a) / exp(x)
 */

static const float _log2 = 0.693147180559945f;

float
logf(float a)
{
	int e;
	a = frexpf(a, &e);
	float l = -_log2;
	if (a != 0.5f) {
		uint8_t loop;
		/* Worst case, we need 5 iterations to converge */
		for (loop = 0; loop < 5; loop++) {
			float ex = expf(l);
			l = l - (ex - a) / ex;
		}
	}
	return l + (float) e * _log2;
}


/* A more accurate version when the exponent is an integer.  This
 * makes integer ** integer give an integer where possible
 */

static inline float
ipow(float a, uint32_t x)
{
	float r = 1;
	while (x) {
		if (x & 1)
			r *= a;
		x >>= 1;
		a *= a;
	}
	return r;
}

float
powf(float a, float b)
{
	uint32_t u = b;
	if (u == b)
		return ipow(a, u);
	return expf(logf(a) * b);
}
