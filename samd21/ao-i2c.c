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
#include <ao-i2c.h>

#define AO_I2C_CLOCK	400000
#define AO_I2C_SERCOM	(&samd21_sercom3)
#define AO_I2C_SERCOM_UNIT	3
#define HAS_I2C_3	1

void
ao_i2c_start(uint8_t addr)
{
	struct samd21_sercom	*sercom = AO_I2C_SERCOM;

	sercom->addr = ((addr << SAMD21_SERCOM_ADDR_ADDR) |
			(0 << SAMD21_SERCOM_ADDR_LENEN) |
			(0 << SAMD21_SERCOM_ADDR_HS) |
			(0 << SAMD21_SERCOM_ADDR_TENBITEN) |
			(0 << SAMD21_SERCOM_ADDR_LEN));
}

void
ao_i2c_send(void *block, uint32_t len, bool stop)
{
	struct samd21_sercom	*sercom = AO_I2C_SERCOM;
	uint8_t	*d = block;

	while (len--) {
		while ((sercom->intflag & (1 << SAMD21_SERCOM_INTFLAG_TXFE)) == 0)
			;
		sercom->data = *d++;
	}
	if (stop) {
		sercom->ctrlb |= (SAMD21_SERCOM_CTRLB_CMD_STOP << SAMD21_SERCOM_CTRLB_CMD);
		while (sercom->syncbusy & (1 << SAMD21_SERCOM_SYNCBUSY_SYSOP))
			;
	}
}

void
ao_i2c_recv(void *block, uint32_t len, bool stop)
{
	struct samd21_sercom	*sercom = AO_I2C_SERCOM;
	uint8_t	*d = block;

	while (len--) {
		while ((sercom->intflag & (1 << SAMD21_SERCOM_INTFLAG_RXFF)) == 0)
			;
		*d++ = sercom->data;
	}
	if (stop) {
		sercom->ctrlb |= (SAMD21_SERCOM_CTRLB_CMD_STOP << SAMD21_SERCOM_CTRLB_CMD);
		while (sercom->syncbusy & (1 << SAMD21_SERCOM_SYNCBUSY_SYSOP))
			;
	}
}

static void
ao_i2c_sercom_init(struct samd21_sercom *sercom, int id)
{
	/* Send a clock along */
	samd21_gclk_clkctrl(0, SAMD21_GCLK_CLKCTRL_ID_SERCOM0_CORE + id);

	/* enable */
	samd21_pm.apbcmask |= (1 << (SAMD21_PM_APBCMASK_SERCOM0 + id));

	/* Reset */
	sercom->ctrla = (1 << SAMD21_SERCOM_CTRLA_SWRST);

	while ((sercom->ctrla & (1 << SAMD21_SERCOM_CTRLA_SWRST)) ||
	       (sercom->syncbusy & (1 << SAMD21_SERCOM_SYNCBUSY_SWRST)))
		;

	/* Configure I2C master mode */
	sercom->ctrla = ((0 << SAMD21_SERCOM_CTRLA_SWRST) |
			 (0 << SAMD21_SERCOM_CTRLA_ENABLE) |
			 (SAMD21_SERCOM_CTRLA_MODE_I2C_MASTER << SAMD21_SERCOM_CTRLA_MODE) |
			 (1 << SAMD21_SERCOM_CTRLA_RUNSTDBY) |

			 (0 << SAMD21_SERCOM_CTRLA_PINOUT) |
			 (SAMD21_SERCOM_CTRLA_SDAHOLD_DIS << SAMD21_SERCOM_CTRLA_SDAHOLD) |
			 (0 << SAMD21_SERCOM_CTRLA_MEXTTOEN) |
			 (0 << SAMD21_SERCOM_CTRLA_SEXTTOEN) |
			 (SAMD21_SERCOM_CTRLA_SPEED_STANDARD << SAMD21_SERCOM_CTRLA_SPEED) |
			 (0 << SAMD21_SERCOM_CTRLA_SCLSM) |
			 (SAMD21_SERCOM_CTRLA_INACTOUT_DIS << SAMD21_SERCOM_CTRLA_INACTOUT) |
			 (0 << SAMD21_SERCOM_CTRLA_LOWTOUT));

	sercom->ctrlb = ((1 << SAMD21_SERCOM_CTRLB_SMEN) |
			 (0 << SAMD21_SERCOM_CTRLB_QCEN) |
			 (SAMD21_SERCOM_CTRLB_CMD_NOP << SAMD21_SERCOM_CTRLB_CMD) |
			 (SAMD21_SERCOM_CTRLB_ACKACT_ACK << SAMD21_SERCOM_CTRLB_ACKACT) |
			 (3 << SAMD21_SERCOM_CTRLB_FIFOCLR));

	sercom->intenclr = 0xff;

	sercom->baud = AO_SYSCLK / AO_I2C_CLOCK - 1;

	/* finish setup and enable the hardware */
	sercom->ctrla |= (1 << SAMD21_SERCOM_CTRLA_ENABLE);
}


void
ao_i2c_init(void)
{
#if HAS_I2C_3
	ao_enable_port(&samd21_port_a);
	samd21_port_pmux_set(&samd21_port_a, 22, SAMD21_PORT_PMUX_FUNC_C);
	samd21_port_pmux_set(&samd21_port_a, 23, SAMD21_PORT_PMUX_FUNC_C);
	ao_i2c_sercom_init(AO_I2C_SERCOM, AO_I2C_SERCOM_UNIT);
#endif
}
