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

#define LIS3DH_ADDR	0x32

#define LIS3DH_CTRL0	0x1e
#define LIS3DH_TEMP_CFG	0x1f
#define LIS3DH_CTRL1	0x20
# define LIS3DH_CTRL1_XEN	0
# define LIS3DH_CTRL1_YEN	1
# define LIS3DH_CTRL1_ZEN	2
# define LIS3DH_CTRL1_LPEN	3
# define LIS3DH_CTRL1_ODR	4
#  define LIS3DH_CTRL1_ODR_1		1
#  define LIS3DH_CTRL1_ODR_10		2
#  define LIS3DH_CTRL1_ODR_25		3
#  define LIS3DH_CTRL1_ODR_50		4
#  define LIS3DH_CTRL1_ODR_100		5

#define LIS3DH_CTRL2	0x21
#define LIS3DH_CTRL3	0x22
#define LIS3DH_CTRL4	0x23
# define LIS3DH_CTRL4_SIM	0
# define LIS3DH_CTRL4_ST	1
# define LIS3DH_CTRL4_HR	3
# define LIS3DH_CTRL4_FS	4
#  define LIS3DH_CTRL4_FS_2		0
#  define LIS3DH_CTRL4_FS_4		1
#  define LIS3DH_CTRL4_FS_8		2
#  define LIS3DH_CTRL4_FS_16		3
# define LIS3DH_CTRL4_BLE	6
# define LIS3DH_CTRL4_BDU	7

#define LIS3DH_CTRL5	0x24
#define LIS3DH_CTRL6	0x25
#define LIS3DH_REF	0x26
#define LIS3DH_STATUS	0x27
#define LIS3DH_OUT_X_L	0x28
#define LIS3DH_OUT_X_H	0x29
#define LIS3DH_OUT_Y_L	0x2a
#define LIS3DH_OUT_Y_H	0x2b
#define LIS3DH_OUT_Z_L	0x2c
#define LIS3DH_OUT_Z_H	0x2d

static uint8_t been_here;

snek_poly_t
snek_builtin_accel(void)
{
	if (!been_here) {
		snek_i2c_put(LIS3DH_ADDR,
			     LIS3DH_CTRL1,
			     _BV(LIS3DH_CTRL1_XEN) |
			     _BV(LIS3DH_CTRL1_YEN) |
			     _BV(LIS3DH_CTRL1_ZEN) |
			     (LIS3DH_CTRL1_ODR_100 << LIS3DH_CTRL1_ODR));

		snek_builtin_time_sleep(snek_float_to_poly(0.1));
		been_here = true;
	}

	snek_list_t *ret_list = snek_list_make(3, snek_list_tuple);
	if (!ret_list)
		return SNEK_NULL;

	snek_poly_t *d = snek_list_data(ret_list);
	for (uint8_t a = 0; a < 6; a += 2) {
		union {
			struct {
				uint8_t l;
				uint8_t h;
			};
			int16_t i;
		} u;
		u.l = snek_i2c_get(LIS3DH_ADDR, LIS3DH_OUT_X_L + a + 0);
		u.h = snek_i2c_get(LIS3DH_ADDR, LIS3DH_OUT_X_L + a + 1);
		*d++ = snek_float_to_poly((float) u.i / 16000.0f);
	}
	return snek_list_to_poly(ret_list);
}
