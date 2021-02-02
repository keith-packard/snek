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

#define TWI_FREQ 400000

static void
snek_i2c_cr(uint8_t val)
{
	TWCR = val;
	while ((TWCR & _BV(TWINT)) == 0)
		;
}

static uint8_t
snek_i2c_read(void)
{
	snek_i2c_cr(_BV(TWINT) | _BV(TWEN));
	return TWDR;
}

static void
snek_i2c_write(uint8_t val)
{
	TWDR = val;
	snek_i2c_cr(_BV(TWINT) | _BV(TWEN));
}

static bool been_here;

static void
snek_i2c_start(uint8_t addr)
{
	if (!been_here) {
		/* Enable pull-ups on PC4 and PC5 */
		PORTC |= (1 << 4) | (1 << 5);

		/* Set pre-scaler and bit rate */
//		TWSR = 0;
		TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
		been_here = true;
	}

	/* Start */
	snek_i2c_cr(_BV(TWINT) | _BV(TWSTA) | _BV(TWEN));
	snek_i2c_write(addr);
}

static void
snek_i2c_stop(void)
{
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

void
snek_i2c_put(uint8_t addr, uint8_t reg, uint8_t val)
{
	snek_i2c_start(addr | SNEK_I2C_WRITE);
	snek_i2c_write(reg);
	snek_i2c_write(val);
	snek_i2c_stop();
}

uint8_t
snek_i2c_get(uint8_t addr, uint8_t reg)
{
	uint8_t val;
	snek_i2c_start(addr | SNEK_I2C_WRITE);
	snek_i2c_write(reg);
	snek_i2c_start(addr | SNEK_I2C_READ);
	val = snek_i2c_read();
	snek_i2c_stop();
	return val;
}
