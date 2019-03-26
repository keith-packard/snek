/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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
/* frexp */
/* fsum */

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
	if (nposition != 2) {
		snek_error("wrong number of args: wanted 2, got %d", nposition);
		return SNEK_NULL;
	}
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
m1(math_isinf, isinff)
m1(math_isnan, isnanf)
m2(math_ldexp, ldexpf)
/* modf */
m2(math_remainder, remainderf)
m1(math_trunc, truncf)
m1(math_round, roundf)

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

static float
_lgammaf(float f)
{
	int sgn;
	return lgammaf_r(f, &sgn);
}

m1(math_lgamma, _lgammaf);

m1(math_cbrt, cbrtf)

