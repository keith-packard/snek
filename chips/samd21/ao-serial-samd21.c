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

#include <ao.h>

static int
_ao_usart_tx_start(struct ao_samd21_usart *usart)
{
	if (!ao_fifo_empty(&usart->tx_fifo)) {
#if HAS_SERIAL_SW_FLOW
		if (usart->gpio_cts && ao_gpio_get(usart->gpio_cts, usart->pin_cts, foo) == 1) {
			ao_exti_enable(usart->gpio_cts, usart->pin_cts);
			return 0;
		}
#endif
		if (usart->reg->intflag & (1 << SAMD21_SERCOM_INTFLAG_DRE))
		{
			usart->tx_running = 1;
			usart->reg->intenset = (1 << SAMD21_SERCOM_INTFLAG_DRE) | (1 << SAMD21_SERCOM_INTFLAG_TXC);
			usart->reg->data = ao_fifo_remove(&usart->tx_fifo);
			ao_wakeup(&usart->tx_fifo);
			return 1;
		}
	}
	return 0;
}

static void
_ao_usart_rx(struct ao_samd21_usart *usart, int is_stdin)
{
	if (usart->reg->intflag & (1 << SAMD21_SERCOM_INTFLAG_RXC)) {
		if (!ao_fifo_full(&usart->rx_fifo)) {
			ao_fifo_insert(&usart->rx_fifo, usart->reg->data);
			ao_wakeup(&usart->rx_fifo);
			if (is_stdin)
				ao_wakeup(&ao_stdin_ready);
#if HAS_SERIAL_SW_FLOW
			/* If the fifo is nearly full, turn off RTS and wait
			 * for it to drain a bunch
			 */
			if (usart->gpio_rts && ao_fifo_mostly(usart->rx_fifo)) {
				ao_gpio_set(usart->gpio_rts, usart->pin_rts, usart->pin_rts, 1);
				usart->rts = 0;
			}
#endif
		} else {
			usart->reg->intenclr = (1 << SAMD21_SERCOM_INTFLAG_RXC);
		}
	}
}

static void
ao_usart_isr(struct ao_samd21_usart *usart, int is_stdin)
{
	_ao_usart_rx(usart, is_stdin);

	if (!_ao_usart_tx_start(usart))
		usart->reg->intenclr = (1 << SAMD21_SERCOM_INTFLAG_DRE);

	if (usart->reg->intflag & (1 << SAMD21_SERCOM_INTFLAG_TXC)) {
		usart->tx_running = 0;
		usart->reg->intenclr = (1 << SAMD21_SERCOM_INTFLAG_TXC);
		if (usart->draining) {
			usart->draining = 0;
			ao_wakeup(&usart->tx_fifo);
		}
	}
}

static const uint32_t ao_usart_speeds[] = {
	[AO_SERIAL_SPEED_4800] = 4800,
	[AO_SERIAL_SPEED_9600] = 9600,
	[AO_SERIAL_SPEED_19200] = 19200,
	[AO_SERIAL_SPEED_57600] = 57600,
	[AO_SERIAL_SPEED_115200] = 115200,
};

static void
ao_usart_set_speed(struct ao_samd21_usart *usart, uint8_t speed)
{
	uint64_t	top = (uint64_t) ao_usart_speeds[speed] << (4 + 16);
	uint16_t	baud = 65536 - (top + AO_SYSCLK/2) / AO_SYSCLK;

	usart->reg->baud = baud;
}

static void
ao_usart_init(struct ao_samd21_usart *usart, int hw_flow, int id)
{
	struct samd21_sercom *reg = usart->reg;

	(void) hw_flow;

	/* Send a clock along */
	samd21_gclk_clkctrl(0, SAMD21_GCLK_CLKCTRL_ID_SERCOM0_CORE + id);

	samd21_nvic_set_enable(SAMD21_NVIC_ISR_SERCOM0_POS + id);
	samd21_nvic_set_priority(SAMD21_NVIC_ISR_SERCOM0_POS + id, 4);

	/* enable */
	samd21_pm.apbcmask |= (1 << (SAMD21_PM_APBCMASK_SERCOM0 + id));

	/* Reset */
	reg->ctrla = (1 << SAMD21_SERCOM_CTRLA_SWRST);

	while ((reg->ctrla & (1 << SAMD21_SERCOM_CTRLA_SWRST)) ||
	       (reg->syncbusy & (1 << SAMD21_SERCOM_SYNCBUSY_SWRST)))
		;

	reg->ctrlb = ((0 << SAMD21_SERCOM_CTRLB_CHSIZE) |
		      (0 << SAMD21_SERCOM_CTRLB_SBMODE) |
		      (0 << SAMD21_SERCOM_CTRLB_COLDEN) |
		      (0 << SAMD21_SERCOM_CTRLB_SFDE) |
		      (0 << SAMD21_SERCOM_CTRLB_ENC) |
		      (0 << SAMD21_SERCOM_CTRLB_PMODE) |
		      (1 << SAMD21_SERCOM_CTRLB_TXEN) |
		      (1 << SAMD21_SERCOM_CTRLB_RXEN) |
		      (3 << SAMD21_SERCOM_CTRLB_FIFOCLR));

	ao_usart_set_speed(usart, AO_SERIAL_SPEED_9600);

	/* finish setup and enable the hardware */
	reg->ctrla = ((0 << SAMD21_SERCOM_CTRLA_SWRST) |
		      (1 << SAMD21_SERCOM_CTRLA_ENABLE) |
		      (1 << SAMD21_SERCOM_CTRLA_MODE) |
		      (1 << SAMD21_SERCOM_CTRLA_RUNSTDBY) |
		      (0 << SAMD21_SERCOM_CTRLA_IBON) |
		      (0 << SAMD21_SERCOM_CTRLA_SAMPR) |
		      (1 << SAMD21_SERCOM_CTRLA_TXPO) |	/* pad[2] */
		      (3 << SAMD21_SERCOM_CTRLA_RXPO) |	/* pad[3] */
		      (0 << SAMD21_SERCOM_CTRLA_SAMPA) |
		      (0 << SAMD21_SERCOM_CTRLA_FORM) |	/* no parity */
		      (0 << SAMD21_SERCOM_CTRLA_CMODE) | /* async */
		      (0 << SAMD21_SERCOM_CTRLA_CPOL) |
		      (1 << SAMD21_SERCOM_CTRLA_DORD));	/* LSB first */

	/* Enable receive interrupt */
	reg->intenset = (1 << SAMD21_SERCOM_INTFLAG_RXC);
}

static int
_ao_usart_pollchar(struct ao_samd21_usart *usart)
{
	int	c;

	if (ao_fifo_empty(&usart->rx_fifo))
		c = AO_READ_AGAIN;
	else {
		uint8_t	u;
		u = ao_fifo_remove(&usart->rx_fifo);
		if ((usart->reg->intenset & (1 << SAMD21_SERCOM_INTFLAG_RXC)) == 0) {
			if (ao_fifo_barely(&usart->rx_fifo))
				usart->reg->intenset = (1 << SAMD21_SERCOM_INTFLAG_RXC);
		}
#if HAS_SERIAL_SW_FLOW
		/* If we've cleared RTS, check if there's space now and turn it back on */
		if (usart->gpio_rts && usart->rts == 0 && ao_fifo_barely(usart->rx_fifo)) {
			ao_gpio_set(usart->gpio_rts, usart->pin_rts, foo, 0);
			usart->rts = 1;
		}
#endif
		c = u;
	}
	return c;
}

static char
ao_usart_getchar(struct ao_samd21_usart *usart)
{
	int c;
	ao_arch_block_interrupts();
	while ((c = _ao_usart_pollchar(usart)) == AO_READ_AGAIN)
		ao_sleep(&usart->rx_fifo);
	ao_arch_release_interrupts();
	return (char) c;
}

static void
ao_usart_putchar(struct ao_samd21_usart *usart, char c)
{
	ao_arch_block_interrupts();
	while (ao_fifo_full(&usart->tx_fifo))
		ao_sleep(&usart->tx_fifo);
	ao_fifo_insert(&usart->tx_fifo, c);
	_ao_usart_tx_start(usart);
	ao_arch_release_interrupts();
}

static void
ao_usart_drain(struct ao_samd21_usart *usart)
{
	ao_arch_block_interrupts();
	while (!ao_fifo_empty(&usart->tx_fifo) || usart->tx_running) {
		usart->draining = 1;
		ao_sleep(&usart->tx_fifo);
	}
	ao_arch_release_interrupts();
}

#if HAS_SERIAL_0

struct ao_samd21_usart ao_samd21_usart0;

void samd21_sercom0_isr(void) { ao_usart_isr(&ao_samd21_usart0, USE_SERIAL_0_STDIN); }

char
ao_serial0_getchar(void)
{
	return ao_usart_getchar(&ao_samd21_usart0);
}

void
ao_serial0_putchar(char c)
{
	ao_usart_putchar(&ao_samd21_usart0, c);
}

int
_ao_serial0_pollchar(void)
{
	return _ao_usart_pollchar(&ao_samd21_usart0);
}

void
ao_serial0_drain(void)
{
	ao_usart_drain(&ao_samd21_usart0);
}

void
ao_serial0_set_speed(uint8_t speed)
{
	ao_usart_drain(&ao_samd21_usart0);
	ao_usart_set_speed(&ao_samd21_usart0, speed);
}
#endif	/* HAS_SERIAL_0 */

void
ao_serial_init(void)
{
#if HAS_SERIAL_0

#if SERIAL_0_PA10_PA11
	/* Pin settings */
	ao_enable_port(&samd21_port_a);
	samd21_port_pmux_set(&samd21_port_a, 10, SAMD21_PORT_PMUX_FUNC_C);
	samd21_port_pmux_set(&samd21_port_a, 11, SAMD21_PORT_PMUX_FUNC_C);
#else
#error "No SERIAL_0 port configuration specified"
#endif

	ao_samd21_usart0.reg = &samd21_sercom0;
	ao_usart_init(&ao_samd21_usart0, 0, 0);

#if USE_SERIAL_0_STDIN
	ao_add_stdio(_ao_serial0_pollchar,
		     ao_serial0_putchar,
		     NULL);
#endif
#endif
}
