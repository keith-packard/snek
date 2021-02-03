/*
 * Copyright © 2021 Keith Packard <keithp@keithp.com>
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
#include "snek-i2c.h"

snek_poly_t
snek_builtin_i2c_put(snek_poly_t addr, snek_poly_t reg, snek_poly_t val)
{
	uint8_t iaddr = snek_poly_get_soffset(addr);
	uint8_t ireg = snek_poly_get_soffset(reg);
	uint8_t ival = snek_poly_get_soffset(val);

	if (snek_abort)
		return SNEK_NULL;

	snek_i2c_put(iaddr, ireg, ival);
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_i2c_get(snek_poly_t addr, snek_poly_t reg)
{
	uint8_t iaddr = snek_poly_get_soffset(addr);
	uint8_t ireg = snek_poly_get_soffset(reg);

	if (snek_abort)
		return SNEK_NULL;

	uint8_t ival = snek_i2c_get(iaddr, ireg);
	return snek_soffset_to_poly(ival);
}
