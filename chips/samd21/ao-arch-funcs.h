/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#ifndef _AO_ARCH_FUNCS_H_
#define _AO_ARCH_FUNCS_H_

#define AO_MODE_PULL_NONE	0
#define AO_MODE_PULL_UP		1
#define AO_MODE_PULL_DOWN	2

static inline void ao_enable_port(struct samd21_port *port)
{
	(void) port;
	samd21_pm.apbbmask |= (1 << SAMD21_PM_APBBMASK_PORT);
}

static inline void ao_disable_port(struct samd21_port *port)
{
	(void) port;
}

static inline void
ao_gpio_set(struct samd21_port *port, uint8_t bit, uint8_t v)
{
	if (v)
		port->outset = (1 << bit);
	else
		port->outclr = (1 << bit);
}

static inline uint8_t
ao_gpio_get(struct samd21_port *port, uint8_t bit)
{
	return (port->in >> bit) & 1;
}

static inline void
ao_gpio_dir_set(struct samd21_port *port, uint8_t bit, bool output)
{
	if (output)
		port->dirset = (1 << bit);
	else
		port->dirclr = (1 << bit);
}

static inline void
ao_enable_output(struct samd21_port *port, uint8_t pin, uint8_t v)
{
	ao_enable_port(port);
	ao_gpio_set(port, pin, v);
	samd21_port_dir_set(port, pin, SAMD21_PORT_DIR_OUT);
	samd21_port_pincfg_set(port, pin,
			       (1 << SAMD21_PORT_PINCFG_DRVSTR) |
			       (1 << SAMD21_PORT_PINCFG_PULLEN) |
			       (1 << SAMD21_PORT_PINCFG_INEN),
			       (0 << SAMD21_PORT_PINCFG_DRVSTR) |
			       (0 << SAMD21_PORT_PINCFG_PULLEN) |
			       (0 << SAMD21_PORT_PINCFG_INEN));
}

static inline void
ao_enable_input(struct samd21_port *port, uint8_t pin, uint32_t mode)
{
	ao_enable_port(port);
	samd21_port_dir_set(port, pin, SAMD21_PORT_DIR_IN);
	uint8_t	pincfg;

	pincfg = ((0 << SAMD21_PORT_PINCFG_DRVSTR) |
		  (0 << SAMD21_PORT_PINCFG_PULLEN) |
		  (1 << SAMD21_PORT_PINCFG_INEN) |
		  (0 << SAMD21_PORT_PINCFG_PMUXEN));

	if (mode != AO_MODE_PULL_NONE) {
		pincfg |= (1 << SAMD21_PORT_PINCFG_PULLEN);
		ao_gpio_set(port, pin, mode == AO_MODE_PULL_UP);
	}

	samd21_port_pincfg_set(port, pin,
			       (1 << SAMD21_PORT_PINCFG_DRVSTR) |
			       (1 << SAMD21_PORT_PINCFG_PULLEN) |
			       (1 << SAMD21_PORT_PINCFG_INEN) |
			       (1 << SAMD21_PORT_PINCFG_PMUXEN),
			       pincfg);
}

static inline void
ao_enable_cs(struct samd21_port *port, uint8_t pin)
{
	ao_enable_output(port, pin, 1);
}

#define ARM_PUSH32(stack, val)	(*(--(stack)) = (val))

typedef uint32_t	ao_arch_irq_t;

static inline uint32_t
ao_arch_irqsave(void) {
	uint32_t	primask;
	asm("mrs %0,primask" : "=&r" (primask));
	ao_arch_block_interrupts();
	return primask;
}

static inline void
ao_arch_irqrestore(uint32_t primask) {
	asm("msr primask,%0" : : "r" (primask));
}

static inline void
ao_arch_memory_barrier(void) {
	asm volatile("" ::: "memory");
}

#define ao_arch_wait_interrupt() do {				\
		asm("\twfi\n");					\
		ao_arch_release_interrupts();			\
		asm(".global ao_idle_loc\nao_idle_loc:");	\
		ao_arch_block_interrupts();			\
	} while (0)

#define ao_arch_critical(b) do {			\
		uint32_t __mask = ao_arch_irqsave();	\
		do { b } while (0);			\
		ao_arch_irqrestore(__mask);		\
	} while (0)

/* ao_serial_samd21.c */

#if USE_SERIAL_0_FLOW && USE_SERIAL_0_SW_FLOW || USE_SERIAL_1_FLOW && USE_SERIAL_1_SW_FLOW
#define HAS_SERIAL_SW_FLOW	1
#else
#define HAS_SERIAL_SW_FLOW	0
#endif

#if USE_SERIAL_1_FLOW && !USE_SERIAL_1_SW_FLOW
#define USE_SERIAL_1_HW_FLOW	1
#endif

#if USE_SERIAL_0_FLOW && !USE_SERIAL_0_SW_FLOW
#define USE_SERIAL_0_HW_FLOW	1
#endif

#if USE_SERIAL_0_HW_FLOW || USE_SERIAL_1_HW_FLOW
#define HAS_SERIAL_HW_FLOW	1
#else
#define HAS_SERIAL_HW_FLOW	0
#endif

struct ao_samd21_usart {
	struct ao_fifo		rx_fifo;
	struct ao_fifo		tx_fifo;
	struct samd21_sercom	*reg;
	uint8_t			tx_running;
	uint8_t			draining;
#if HAS_SERIAL_SW_FLOW
	/* RTS - 0 if we have FIFO space, 1 if not
	 * CTS - 0 if we can send, 0 if not
	 */
	struct samd21_port	*gpio_rts;
	struct samd21_port	*gpio_cts;
	uint8_t			pin_rts;
	uint8_t			pin_cts;
	uint8_t			rts;
#endif
};

#if HAS_USART_0
extern struct ao_samd21_usart	ao_samd21_usart0;
#endif

void
ao_serial_init(void);

/* ao_usb_samd21.c */

#if AO_USB_OUT_HOOK
void
ao_usb_out_hook(uint8_t *buffer, uint16_t count);
#endif

void start(void);

#endif /* _AO_ARCH_FUNCS_H_ */
