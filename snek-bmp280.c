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

#define BMP280_ADDR	0xee

#define BMP280_T1	0x88
#define BMP280_T2	0x8a
#define BMP280_T3	0x8c
#define BMP280_P1	0x8e
#define BMP280_P2	0x90
#define BMP280_P3	0x92
#define BMP280_P4	0x94
#define BMP280_P5	0x96
#define BMP280_P6	0x98
#define BMP280_P7	0x9a
#define BMP280_P8	0x9c
#define BMP280_P9	0x9e

#define BMP280_ID	0xd0

#define BMP280_CTRL	0xf4
#define BMP280_PRES	0xf7
#define BMP280_TEMP	0xfa

static uint8_t
get8(uint8_t reg)
{
	return snek_i2c_get(BMP280_ADDR, reg);
}

static int32_t
sample(uint8_t reg)
{
	uint32_t u;

	u = (uint32_t) get8(reg) << 16;
	u |= (uint32_t) get8(reg+1) << 8;
	u |= (uint32_t) get8(reg+2);
	u >>= 4;
	return (int32_t) u;
}

static struct {
	uint16_t	t1;
	int16_t		t2;
	int16_t		t3;
	uint16_t	p1;
	int16_t		p2;
	int16_t		p3;
	int16_t		p4;
	int16_t		p5;
	int16_t		p6;
	int16_t		p7;
	int16_t		p8;
	int16_t		p9;
} rom;

#define dig_T1 ((int32_t) rom.t1)
#define dig_T2 ((int32_t) rom.t2)
#define dig_T3 ((int32_t) rom.t3)
#define dig_P1 ((int32_t) rom.p1)
#define dig_P2 ((int32_t) rom.p2)
#define dig_P3 ((int32_t) rom.p3)
#define dig_P4 ((int32_t) rom.p4)
#define dig_P5 ((int32_t) rom.p5)
#define dig_P6 ((int32_t) rom.p6)
#define dig_P7 ((int32_t) rom.p7)
#define dig_P8 ((int32_t) rom.p8)
#define dig_P9 ((int32_t) rom.p9)

snek_poly_t
snek_builtin_baro(void)
{
	/* normal mode */
	snek_i2c_put(BMP280_ADDR, BMP280_CTRL, 0x3f);

	/* Read the ROM */
	uint8_t	r;
	uint8_t	*b = (uint8_t *) &rom;

	for (r = BMP280_T1; r <= BMP280_P9 + 1; r++)
		*b++ = get8(r);

#if 0
	int32_t adc_T = sample(BMP280_TEMP);

	float var1, var2, T;
	var1 = (((float)adc_T)/16384.0f - ((float)dig_T1)/1024.0f) * ((float)dig_T2);
	var2 = ((((float)adc_T)/131072.0f - ((float)dig_T1)/8192.0f) *
		(((float)adc_T)/131072.0f - ((float) dig_T1)/8192.0f)) * ((float)dig_T3);
	float t_fine = (var1 + var2);
//	T = (var1 + var2) / 5120.0f;

	/* Get pressure */
	int32_t adc_P = sample(BMP280_PRES);

	float p;
	var1 = ((float)t_fine/2.0f) - 64000.0f;
	var2 = var1 * var1 * ((float)dig_P6) / 32768.0f;
	var2 = var2 + var1 * ((float)dig_P5) * 2.0f;
	var2 = (var2/4.0f)+(((float)dig_P4) * 65536.0f);
	var1 = (((float)dig_P3) * var1 * var1 / 524288.0f + ((float)dig_P2) * var1) / 524288.0f;
	var1 = (1.0f + var1 / 32768.0f)*((float)dig_P1);
	if (var1 == 0.0f)
	{
		p = 0.0f; // avoid exception caused by division by zero
	}
	else
	{
		p = 1048576.0f - (float)adc_P;
		p = (p - (var2 / 4096.0f)) * 6250.0f / var1;
		var1 = ((float)dig_P9) * p * p / 2147483648.0f;
		var2 = p * ((float)dig_P8) / 32768.0f;
		p = p + (var1 + var2 + ((float)dig_P7)) / 16.0f;
	}
	return snek_float_to_poly(p);
#else

	/* Get temperature */
	int32_t t_fine;
	{
		int32_t adc_T = sample(BMP280_TEMP);
		int32_t var1 = ((((adc_T>>3) - (dig_T1<<1))) * (dig_T2)) >> 11;
		int32_t var2 = (((((adc_T>>4) - (dig_T1)) * ((adc_T>>4) - (dig_T1))) >> 12) *
				(dig_T3)) >> 14;
		t_fine = var1 + var2;
	}

	/* Get pressure */
	int32_t adc_P = sample(BMP280_PRES);

#if 1
	uint32_t p;
	{
		int32_t var1, var2;

		var1 = ((t_fine)>>1) - 64000;
		var2 = (((var1>>2) * (var1>>2)) >> 11 ) * (dig_P6);
		var2 = var2 + ((var1*(dig_P5))<<1);
		var2 = (var2>>2)+((dig_P4)<<16);
		var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + (((dig_P2) * var1)>>1))>>18;
		var1 = ((((32768+var1))*(dig_P1))>>15);
		if (var1 == 0) {
			p = 0;
		} else {
			p = (((uint32_t)(((int32_t)1048576L)-adc_P)-(var2>>12)))*3125;
			if (p < 0x80000000L)
				p = (p << 1) / ((uint32_t)var1);
			else
				p = (p / (uint32_t)var1) * 2;
			var1 = ((dig_P9) * ((((p>>3) * (p>>3))>>13)))>>12;
			var2 = (((p>>2)) * (dig_P8))>>13;
			p = (uint32_t)((int32_t) p + ((var1 + var2 + dig_P7) >> 4));
		}
	}
#else
	int64_t p;
	{
		int64_t var1, var2;
		var1 = ((int64_t)t_fine) - 128000;
		var2 = var1 * var1 * (int64_t)dig_P6;
		var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
		var2 = var2 + (((int64_t)dig_P4)<<35);
		var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
		var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
		if (var1 == 0)
		{
			p = 0;
		} else {
			p = 1048576-adc_P;
			p = (((p<<31)-var2)*3125)/var1;
			var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
			var2 = (((int64_t)dig_P8) * p) >> 19;
			p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
		}
	}
#endif
	return snek_float_to_poly((float) p);
#endif
}
