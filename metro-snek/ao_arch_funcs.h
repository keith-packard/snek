/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#define AO_EXTI_MODE_PULL_NONE	0
#define AO_EXTI_MODE_PULL_UP	1
#define AO_EXTI_MODE_PULL_DOWN	2

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
ao_enable_output(struct samd21_port *port, uint8_t bit, uint8_t v)
{
	ao_enable_port(port);
	ao_gpio_set(port, bit, v);
	samd21_port_dir_set(port, bit, SAMD21_PORT_DIR_OUT);
}

static inline void
ao_gpio_set_mode(struct samd21_port *port, uint8_t bit, uint32_t mode)
{
	uint8_t	pincfg = samd21_port_pincfg_get(port, bit);
	pincfg &= ~(1 << SAMD21_PORT_PINCFG_PULLEN);
	if (mode != AO_EXTI_MODE_PULL_NONE) {
		pincfg |= (1 << SAMD21_PORT_PINCFG_PULLEN);
		ao_gpio_set(port, bit, mode == AO_EXTI_MODE_PULL_UP);
	}
	samd21_port_pincfg_set(port, bit, pincfg);
}

static inline void
ao_enable_input(struct samd21_port *port, uint8_t bit, uint32_t mode)
{
	ao_enable_port(port);
	samd21_port_dir_set(port, bit, SAMD21_PORT_DIR_IN);
	samd21_port_pincfg_set(port, bit,
			       ((0 << SAMD21_PORT_PINCFG_PMUXEN) |
				(1 << SAMD21_PORT_PINCFG_INEN) |
				(0 << SAMD21_PORT_PINCFG_DRVSTR)));
	ao_gpio_set_mode(port, bit, mode);
}

static inline void
ao_enable_cs(struct samd21_port *port, uint8_t bit)
{
	ao_enable_output(port, bit, 1);
}

#define ao_spi_init_cs(port, mask) do {				\
		if ((mask) & 0x0001) ao_enable_cs(port, 0);	\
		if ((mask) & 0x0002) ao_enable_cs(port, 1);	\
		if ((mask) & 0x0004) ao_enable_cs(port, 2);	\
		if ((mask) & 0x0008) ao_enable_cs(port, 3);	\
		if ((mask) & 0x0010) ao_enable_cs(port, 4);	\
		if ((mask) & 0x0020) ao_enable_cs(port, 5);	\
		if ((mask) & 0x0040) ao_enable_cs(port, 6);	\
		if ((mask) & 0x0080) ao_enable_cs(port, 7);	\
		if ((mask) & 0x0100) ao_enable_cs(port, 8);	\
		if ((mask) & 0x0200) ao_enable_cs(port, 9);	\
		if ((mask) & 0x0400) ao_enable_cs(port, 10);\
		if ((mask) & 0x0800) ao_enable_cs(port, 11);\
		if ((mask) & 0x1000) ao_enable_cs(port, 12);\
		if ((mask) & 0x2000) ao_enable_cs(port, 13);\
		if ((mask) & 0x4000) ao_enable_cs(port, 14);\
		if ((mask) & 0x8000) ao_enable_cs(port, 15);\
	} while (0)

/* ao_dma_stm.c
 */

extern uint8_t ao_dma_done;

void
ao_dma_set_transfer(uint8_t 		index,
		    volatile void	*peripheral,
		    void		*memory,
		    uint16_t		count,
		    uint32_t		ccr);

void
ao_dma_set_isr(uint8_t index, void (*isr)(int index));

void
ao_dma_start(uint8_t index);

void
ao_dma_done_transfer(uint8_t index);

void
ao_dma_abort(uint8_t index);

void
ao_dma_alloc(uint8_t index);

void
ao_dma_init(void);

/* ao_i2c_stm.c */

void
ao_i2c_get(uint8_t i2c_index);

uint8_t
ao_i2c_start(uint8_t i2c_index, uint16_t address);

void
ao_i2c_put(uint8_t i2c_index);

uint8_t
ao_i2c_send(void *block, uint16_t len, uint8_t i2c_index, uint8_t stop);

uint8_t
ao_i2c_recv(void *block, uint16_t len, uint8_t i2c_index, uint8_t stop);

void
ao_i2c_init(void);

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

#if HAS_TASK
static inline void
ao_arch_init_stack(struct ao_task *task, void *start)
{
	uint32_t	*sp = &task->stack32[AO_STACK_SIZE >> 2];
	uint32_t	a = (uint32_t) start;
	int		i;

	/* Return address (goes into LR) */
	ARM_PUSH32(sp, a);

	/* Clear register values r0-r7 */
	i = 8;
	while (i--)
		ARM_PUSH32(sp, 0);

	/* APSR */
	ARM_PUSH32(sp, 0);

	/* PRIMASK with interrupts enabled */
	ARM_PUSH32(sp, 0);

	task->sp32 = sp;
}

static inline void ao_arch_save_regs(void) {
	/* Save general registers */
	asm("push {r0-r7,lr}\n");

	/* Save APSR */
	asm("mrs r0,apsr");
	asm("push {r0}");

	/* Save PRIMASK */
	asm("mrs r0,primask");
	asm("push {r0}");
}

static inline void ao_arch_save_stack(void) {
	uint32_t	*sp;
	asm("mov %0,sp" : "=&r" (sp) );
	ao_cur_task->sp32 = (sp);
	if (sp < &ao_cur_task->stack32[0])
		ao_panic (AO_PANIC_STACK);
}

static inline void ao_arch_restore_stack(void) {
	/* Switch stacks */
	asm("mov sp, %0" : : "r" (ao_cur_task->sp32) );

	/* Restore PRIMASK */
	asm("pop {r0}");
	asm("msr primask,r0");

	/* Restore APSR */
	asm("pop {r0}");
	asm("msr apsr_nczvq,r0");

	/* Restore general registers */
	asm("pop {r0-r7,pc}\n");
}

#ifndef HAS_SAMPLE_PROFILE
#define HAS_SAMPLE_PROFILE 0
#endif

#if !HAS_SAMPLE_PROFILE
#define HAS_ARCH_START_SCHEDULER	1

static inline void ao_arch_start_scheduler(void) {
	uint32_t	sp;
	uint32_t	control;

	asm("mrs %0,msp" : "=&r" (sp));
	asm("msr psp,%0" : : "r" (sp));
	asm("mrs %0,control" : "=&r" (control));
	control |= (1 << 1);
	asm("msr control,%0" : : "r" (control));
	asm("isb");
}
#endif

#define ao_arch_isr_stack()

#endif

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
	struct samd21_usart	*reg;
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

#if AO_USB_DIRECTIO
uint8_t
ao_usb_alloc(uint16_t *buffers[2]);

uint8_t
ao_usb_alloc2(uint16_t *buffers[2]);

uint8_t
ao_usb_write(uint16_t len);

uint8_t
ao_usb_write2(uint16_t len);
#endif /* AO_USB_DIRECTIO */

void start(void);

void
ao_debug_out(char c);

#endif /* _AO_ARCH_FUNCS_H_ */
