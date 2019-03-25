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
#include <ao_pins.h>
#include <ao_arch.h>
#include <ao_notask.h>
#include <ao_serial.h>

extern int ao_putchar(char c);
extern char ao_getchar(void);

#define AO_MAX_VERSION		8

typedef AO_PORT_TYPE ao_port_t;

/*
 * ao_panic.c
 */

#define AO_PANIC_STDIO		7	/* Too many stdio handlers registered */
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
 * ao_mutex.c
 */

#ifndef ao_mutex_get
uint8_t
ao_mutex_try(uint8_t *ao_mutex, uint8_t task_id);

void
ao_mutex_get(uint8_t *ao_mutex);

void
ao_mutex_put(uint8_t *ao_mutex);
#endif

/*
 * Various drivers
 */

#if LEDS_AVAILABLE || HAS_LED
#include <ao_led.h>
#endif

#if HAS_USB
#include <ao_usb.h>
#endif

/*
 * ao_stdio.c
 */

#define AO_READ_AGAIN	(-1)

struct ao_stdio {
	int	(*_pollchar)(void);	/* Called with interrupts blocked */
	void	(*putchar)(char c);
	void	(*flush)(void);
	uint8_t	echo;
};

int
ao_putc(char c, FILE *ignore);

int
ao_getc(FILE *ignore);

int
ao_flushc(FILE *ignore);

extern struct ao_stdio ao_stdios[];
extern int8_t ao_cur_stdio;
extern int8_t ao_num_stdios;

void
flush(void);

extern uint8_t ao_stdin_ready;

uint8_t
ao_echo(void);

int8_t
ao_add_stdio(int (*pollchar)(void),
	     void (*putchar)(char) ,
	     void (*flush)(void));

/*
 * ao_ignite.c
 */

enum ao_igniter {
	ao_igniter_drogue = 0,
	ao_igniter_main = 1
};

void
ao_ignite(enum ao_igniter igniter);

enum ao_igniter_status {
	ao_igniter_unknown,	/* unknown status (ambiguous voltage) */
	ao_igniter_ready,	/* continuity detected */
	ao_igniter_active,	/* igniter firing */
	ao_igniter_open,	/* open circuit detected */
};

struct ao_ignition {
	uint8_t	request;
	uint8_t fired;
	uint8_t firing;
};

extern const char * const ao_igniter_status_names[];

extern struct ao_ignition ao_ignition[2];

enum ao_igniter_status
ao_igniter_status(enum ao_igniter igniter);

extern uint8_t ao_igniter_present;

void
ao_ignite_set_pins(void);

void
ao_igniter_init(void);

/*
 * Fifos
 */

#ifndef AO_FIFO_SIZE
#if USE_USB_FIFO
#define AO_FIFO_SIZE	256
#else
#define AO_FIFO_SIZE	32
#endif
#endif

#if AO_FIFO_SIZE <= 255
typedef uint8_t	fifo_t;
#else
typedef uint16_t fifo_t;
#endif

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
	char	c = f->fifo[f->remove++];
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

#if PACKET_HAS_MASTER || PACKET_HAS_SLAVE
#include <ao_packet.h>
#endif

#if HAS_BTM
#include <ao_btm.h>
#endif

#if HAS_COMPANION
#include <ao_companion.h>
#endif

#if HAS_LCD
#include <ao_lcd.h>
#endif

#if HAS_AES
#include <ao_aes.h>
#endif

#include <ao_arch_funcs.h>

#endif /* _AO_H_ */
