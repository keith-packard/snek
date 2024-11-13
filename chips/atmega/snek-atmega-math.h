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

#ifndef _SNEK_MEGA_MATH_H_
#define _SNEK_MEGA_MATH_H_

#undef float

float exp2f(float x);

float expm1f(float x);

#define __ieee754_remainderf remainderf
#define __ieee754_acoshf acoshf
#define __ieee754_atanhf atanhf
#define __ieee754_sqrtf sqrtf
#define __ieee754_lgammaf_r lgammaf_r
#define __ieee754_gammaf_r gammaf_r

#define _IEEE_LIBM

double acosh(double x);
double asinh(double x);
double atanh(double x);
double log1p(double x);
double log2(double x);
double erf(double x) ;
double erfc(double x);
double tgamma(double x);

float acoshf(float x);
float asinhf(float x);
float atanhf(float x);
float log1pf(float x);
float log2f(float x);
float erff(float x);
float erfcf(float x);
float tgammaf(float x);
float remainderf(float x, float y);
float lgammaf_r(float x, int *signp);

#define __math_invalidf(x) (x)
#define __math_divzerof(x) (NAN)

#endif /* _SNEK_MEGA_MATH_H_ */
