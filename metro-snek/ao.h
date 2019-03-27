/*
 * Copyright © 2009 Keith Packard <keithp@keithp.com>
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

#ifndef _AO_H_
#define _AO_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <ao-pins.h>
#include <ao-arch.h>
#include <ao-notask.h>
#include <ao-serial.h>

#define AO_MAX_VERSION		8

/*
 * ao_panic.c
 */

#define AO_PANIC_CRASH		14	/* Processor crashed */

/* Stop the operating system, beeping and blinking the reason */
void
ao_panic(uint8_t reason);

/*
 * ao_timer.c
 */

#ifndef AO_TICK_TYPE
#define AO_TICK_TYPE	uint32_t
#define AO_TICK_SIGNED	int32_t
#endif

extern volatile AO_TICK_TYPE ao_tick_count;

/* Our timer runs at 100Hz */
#ifndef AO_HERTZ
#define AO_HERTZ		100
#endif
#define AO_MS_TO_TICKS(ms)	((ms) / (1000 / AO_HERTZ))
#define AO_SEC_TO_TICKS(s)	((s) * AO_HERTZ)

/* Returns the current time in ticks */
AO_TICK_TYPE
ao_time(void);

/* Returns the current time in ns */
uint64_t
ao_time_ns(void);

/* Suspend the current task until ticks time has passed */
void
ao_delay(uint16_t ticks);

/* Initialize the timer */
void
ao_timer_init(void);

/* Initialize the hardware clock. Must be called first */
void
ao_clock_init(void);

#if AO_POWER_MANAGEMENT
/* Go to low power clock */
void
ao_clock_suspend(void);

/* Restart full-speed clock */
void
ao_clock_resume(void);
#endif

/*
 * Various drivers
 */

#if LEDS_AVAILABLE || HAS_LED
#include <ao-led.h>
#endif

#if HAS_USB
#include <ao-usb.h>
#endif

/*
 * ao_stdio.c
 */

#define AO_READ_AGAIN	(-1)

extern uint8_t ao_stdin_ready;

/*
 * Fifos
 */

#ifndef AO_FIFO_SIZE
#define AO_FIFO_SIZE	256
#endif

typedef uint16_t fifo_t;

struct ao_fifo {
	fifo_t	remove;
	fifo_t	count;
	char	fifo[AO_FIFO_SIZE];
};

static inline void
ao_fifo_insert(struct ao_fifo *f, char c)
{
	fifo_t insert = (f->remove + f->count) & (AO_FIFO_SIZE - 1);
	f->fifo[insert] = c;
	f->count++;
}

static inline char
ao_fifo_remove(struct ao_fifo *f)
{
	char	c = f->fifo[f->remove];
	f->remove = (f->remove + 1) & (AO_FIFO_SIZE - 1);
	f->count--;
	return c;
}

static inline bool
ao_fifo_full(struct ao_fifo *f)
{
	return f->count == AO_FIFO_SIZE;
}

static inline bool
ao_fifo_empty(struct ao_fifo *f)
{
	return f->count == 0;
}

static inline fifo_t
ao_fifo_space(struct ao_fifo *f)
{
	return f->count;
}

static inline bool
ao_fifo_has_space(struct ao_fifo *f, fifo_t amount)
{
	return amount <= AO_FIFO_SIZE - f->count;
}

static inline bool
ao_fifo_mostly(struct ao_fifo *f)
{
	return f->count >= AO_FIFO_SIZE * 3 / 4;
}

static inline bool
ao_fifo_barely(struct ao_fifo *f)
{
	return f->count <= AO_FIFO_SIZE * 1 / 4;
}

#include <ao-arch-funcs.h>

#endif /* _AO_H_ */
