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

#include "snek.h"
#include <math.h>

#define make_builtin(name) snek_builtin_ ## name

#define m1(name,func) snek_poly_t make_builtin(name)(snek_poly_t a) { return snek_float_to_poly(func(snek_poly_to_float(a))); }
#define m2(name,func) snek_poly_t make_builtin(name)(snek_poly_t a, snek_poly_t b) { return snek_float_to_poly(func(snek_poly_to_float(a), snek_poly_to_float(b))); }

static snek_poly_t
make_tuple(float a, float b)
{
	snek_list_t	*tuple = snek_list_make(2, snek_list_tuple);
	if (!tuple)
		return SNEK_NULL;
	snek_poly_t	*data = snek_list_data(tuple);
	data[0] = snek_float_to_poly(a);
	data[1] = snek_float_to_poly(b);
	return snek_list_to_poly(tuple);
}

m1(math_ceil, ceilf)
m2(math_copysign, copysignf)
m1(math_fabs, fabsf)

static float
factorialf(float f)
{
	float	result = 1;
	if (f > 35)
		return (float) INFINITY;
	for (float x = 2; x <= f; x++)
		result *= x;
	return result;
}

m1(math_factorial, factorialf)
m1(math_floor, floorf)
m2(math_fmod, fmodf)

snek_poly_t
snek_builtin_math_frexp(snek_poly_t a) { int exp; float f = frexpf(snek_poly_to_float(a), &exp); return make_tuple(f, exp); }

static float
getf(snek_list_t *partials, snek_offset_t o) {
	return snek_poly_to_float(snek_list_data(partials)[o]);
}

static void
putf(snek_list_t *partials, snek_offset_t o, float v) {
	snek_list_data(partials)[o] = snek_float_to_poly(v);
}

/*
 * Borrowed from the python3 implementation, this keeps a
 * full-precision sum by storing partial sums in an array.  Note the
 * use of 'volatile' to ensure that the compiler doesn't optimize away
 * some operations.
 */
snek_poly_t
snek_builtin_math_fsum(snek_poly_t a)
{
	snek_list_t *l = snek_poly_to_list(a);
	if (!l)
		return SNEK_NULL;

	snek_offset_t size = l->size;
	snek_offset_t o = 0;
	snek_offset_t s = 1;
	if (snek_list_type(l) == snek_list_dict) {
		o++;
		s = 2;
	}
	float t, x, y;
	float sum = 0.0f;
	snek_offset_t i, j, n, m;
	n = 0;
	m = 4;
	snek_stack_push_list(l);
	snek_list_t *partials = snek_list_make(m, snek_list_list);
	l = snek_stack_pop_list();
	if (!partials)
		return SNEK_NULL;

	volatile float hi, yr, lo;

	for (o = 0; o < size; o += s) {
		x = getf(l, o);

		i = 0;
		for (j = 0; j < n; j++) {
			y = getf(partials, j);
			if (fabsf(x) < fabsf(y)) {
				t = x;
				x = y;
				y = t;
			}
			hi = x + y;
			yr = hi - x;
			lo = y - yr;
			if (lo != 0.0f)
				putf(partials, i++, lo);
			x = hi;
		}

		n = i;
		if (x != 0.0f) {
			if (n >= m) {
				snek_stack_push_list(l);
				partials = snek_list_resize(partials, (m = n + 1));
				l = snek_stack_pop_list();
				if (!partials)
					return SNEK_NULL;
			}
			putf(partials, n++, x);
		}

	}

	hi = 0.0f;
	if (n > 0) {
		hi = getf(partials, --n);
		while (n > 0) {
			x = hi;
			y = getf(partials, --n);
			hi = x + y;
			yr = hi  - x;
			lo = y - yr;
			if (lo != 0.0f)
				break;
		}

		if (n > 0 && ((lo < 0.0f && getf(partials, n-1) < 0.0f) ||
			      (lo > 0.0f && getf(partials, n-1) > 0.0))) {
			y = lo * 2.0f;
			x = hi + y;
			yr = x - hi;
			if (y == yr)
				hi = x;
		}
	}
	sum = hi;

	return snek_float_to_poly(sum);
}

static float
gcdf(float af, float bf)
{
	int	a = fabsf(af);
	int	b = fabsf(bf);

	if (a == 0 || b == 0)
		return 0.0f;
	while (a > 1 && b > 1) {
		int r = a % b;
		a = b;
		b = r;
	}
	return (float) a;
}

m2(math_gcd, gcdf)

snek_poly_t
snek_builtin_math_isclose(uint8_t nposition, uint8_t nnamed, snek_poly_t *args)
{
	if (nposition != 2)
		return snek_error_args(2, nposition);

	float	af = snek_poly_to_float(*args++);
	float	bf = snek_poly_to_float(*args++);
	float	rel_tol = 1e-6f;
	float	abs_tol = 0.0f;

	while (nnamed--) {
		snek_id_t id = (snek_id_t) ((*args++).f);
		float	v = snek_poly_to_float(*args++);
		switch (id) {
		case SNEK_BUILTIN_rel_tol:
			rel_tol = v;
			break;
		case SNEK_BUILTIN_abs_tol:
			abs_tol = v;
			break;
		}
	}
	float dist = fabsf(af - bf);
	float tol = fmaxf(rel_tol * fmaxf(fabsf(af), fabsf(bf)), abs_tol);
	return snek_bool_to_poly(dist <= tol);
}

m1(math_isfinite, isfinite)
#if defined(WIN32) || defined(__APPLE__)
m1(math_isinf, isinf)
#else
m1(math_isinf, isinff)
#endif
m1(math_isnan, isnanf)
m2(math_ldexp, ldexpf)
snek_poly_t
snek_builtin_math_modf(snek_poly_t a) { float i; float f = modff(snek_poly_to_float(a), &i); return make_tuple(f, i); }
m2(math_remainder, remainderf)
m1(math_trunc, truncf)
m1(round, roundf)

m1(math_exp, expf)
m1(math_expm1, expm1f)
m1(math_exp2, exp2f)
m1(math_log, logf)
m1(math_log1p, log1pf)
m1(math_log2, log2f)
m1(math_log10, log10f)
m2(math_pow, powf)

m1(math_acos, acosf)
m1(math_asin, asinf)
m1(math_atan, atanf)
m2(math_atan2, atan2f)
m1(math_cos, cosf)
m2(math_hypot, hypotf)
m1(math_sin, sinf)
m1(math_tan, tanf)

static float
degreesf(float x) { return x * 180.0f / M_PI; }

static float
radiansf(float x) { return x * M_PI / 180.0f; }

m1(math_degrees, degreesf)
m1(math_radians, radiansf)

m1(math_acosh, acoshf);
m1(math_asinh, asinhf);
m1(math_atanh, atanhf);
m1(math_cosh, coshf);
m1(math_sinh, sinhf);
m1(math_tanh, tanhf);

m1(math_erf, erff);
m1(math_erfc, erfcf);
m1(math_gamma, tgammaf);

#if defined(WIN32) || defined(__APPLE__) || defined(_PICOLIBC__)
m1(math_lgamma, lgammaf);
#else
static float
_lgammaf(float f)
{
	int sgn;
	return lgammaf_r(f, &sgn);
}

m1(math_lgamma, _lgammaf);
#endif
