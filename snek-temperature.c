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

#include <ao.h>
#include <ao-snek.h>
#include <snek.h>
#include <math.h>

#define C_TO_K(c)	((c) + 273.15f)
#define K_TO_C(k)	((k) - 273.15f)

snek_poly_t
snek_builtin_temperature(snek_poly_t adc)
{
	float f = snek_poly_get_float(adc);
	float r;

	if (f == 0)
		r = INFINITY;
	else {
#if SNEK_THERM_HIGH_SIDE
		/* Thermistor is connected from Vcc to analog input with r_divide to ground */
		r = SNEK_THERM_R_DIVIDE / f - SNEK_THERM_R_DIVIDE;
#else
		/* Thermistor is connected from ground to analog input with r_divide to Vcc */
		r = SNEK_THERM_R_DIVIDE / (1.0f / f - 1);
#endif
	}

	float k = 1/(logf(r / SNEK_THERM_R_NOMINAL) / SNEK_THERM_B_COEFF + 1.0f / C_TO_K(SNEK_THERM_T_NOMINAL));
	return snek_float_to_poly(K_TO_C(k));
}
