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

/*
 * ao_spi.c
 */

extern __xdata uint8_t	ao_spi_mutex;

#define ao_spi_get_mask(reg,mask,bus,speed) do {	\
		ao_mutex_get(&ao_spi_mutex);	\
		(reg) &= ~(mask);		\
	} while (0)

#define ao_spi_put_mask(reg,mask,bus) do {	\
		(reg) |= (mask);		\
		ao_mutex_put(&ao_spi_mutex);	\
	} while (0)

#define ao_spi_get_bit(reg,bit,pin,bus,speed) do {	\
		ao_mutex_get(&ao_spi_mutex);	\
		(pin) = 0;			\
	} while (0)

#define ao_spi_put_bit(reg,bit,pin,bus) do {	\
		(pin) = 1;			\
		ao_mutex_put(&ao_spi_mutex);	\
	} while (0)


#define ao_gpio_token_paster(x,y)		x ## y
#define ao_gpio_token_evaluator(x,y)	ao_gpio_token_paster(x,y)

#define ao_gpio_set(port, bit, pin, v) do {				\
		if (v)							\
			(ao_gpio_token_evaluator(PORT,port)) |= (1 << bit); \
		else							\
			(ao_gpio_token_evaluator(PORT,port)) &= ~(1 << bit); \
	} while (0)

/*
 * The SPI mutex must be held to call either of these
 * functions -- this mutex covers the entire SPI operation,
 * from chip select low to chip select high
 */

#define ao_enable_output(port, bit, pin, v) do {			\
		ao_gpio_set(port, bit, pin, v);				\
		ao_gpio_token_evaluator(DDR,port) |= (1 << bit);	\
	} while (0)


void
ao_spi_send_bus(void __xdata *block, uint16_t len) __reentrant;

void
ao_spi_recv_bus(void __xdata *block, uint16_t len) __reentrant;

#define ao_spi_send(block, len, bus) ao_spi_send_bus(block, len)
#define ao_spi_recv(block, len, bus) ao_spi_recv_bus(block, len)

void
ao_spi_init(void);

#define ao_spi_init_cs(port, mask) do {		\
		SPI_CS_PORT |= (mask);		\
		SPI_CS_DIR |= (mask);		\
	} while (0)
