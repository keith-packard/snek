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

#ifndef _SNECK_DUINO_H_
#define _SNECK_DUINO_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#define SNEK_DEBUG	0

#ifdef __AVR_PM_BASE_ADDRESS__
#define CONST const
#else
#define PARSE_TABLE_DECLARATION(t) 	__flash t
#define CONST const __flash

#define snek_error_name snek_internal_error
#define snek_error(fmt, args...) ({				\
		static CONST char __fmt__[] = (fmt);	\
		snek_internal_error(__fmt__, ## args);		\
	})

#define snek_error_0_name snek_internal_error_0
#define snek_error_0(string) ({						\
			static CONST char __string__[] = (string); \
			snek_internal_error_0(__string__);		\
		})

#define fprintf(file, fmt, args...) ({				\
		static CONST char __fmt__[] = (fmt);	\
		printf_P((const char *) (uintptr_t) __fmt__, ## args);	\
	})

#define sprintf_const(dst, fmt, args...) ({				\
			static CONST char __fmt__[] = (fmt);		\
			sprintf_P(dst, (const char *) (uintptr_t) __fmt__, ##args); \
		})

#define trailing(next, wo_op, wo, w_op, w) ({				\
			static CONST char __next__[] = (next);		\
			trailing_internal(__next__, wo_op, wo, w_op, w); \
		})
#define trailing_name trailing_internal


#define snek_const_strcmp(a,b)		strcmp_P(a,(const char *) (uintptr_t) (b))

#define snek_const_strcpy(a,b) ({ \
		static CONST char __str__[] = (b); \
		strcpy_P(a, (const char *) (uintptr_t) (__str__)); \
	})

static inline const char *
avr_snek_builtin_names_return(CONST uint8_t *bits)
{
	static char ret[SNEK_BUILTIN_NAMES_MAX_LEN + 1];

	return strcpy_P(ret, (const char *) (uintptr_t) bits);
}

#define snek_builtin_names_return(a) avr_snek_builtin_names_return(a)
#define snek_builtin_names_len(a) strnlen_P((const char *) (uintptr_t) (a), SNEK_BUILTIN_NAMES_MAX_LEN+1)

//#define strfromf_const(dst, len, fmt, val) sprintf_const(dst, fmt, val)
//#define strfromf(dst, len, fmt, val) sprintf(dst, fmt, val)
#endif /* !__AVR_PM_BASE_ADDRESS__ */

float atoff(const char *);
#define strtof(s, n) atoff(s)

/* no sense linking both functions */
#define memcpy(a,b,c) memmove(a,b,c)
#define printf(a,args...) fprintf(stdout, a, ##args)

int
strfromg(char *dst0, float val);

#define strfromf(dst, len, fmt, val) strfromg(dst, val)

void
snek_uart_init(void);

int
snek_uart_putchar(char c, FILE *stream);

int
snek_uart_getchar(FILE *stream);

int
snek_eeprom_getchar(FILE *stream);

#define TICKS_PER_SECOND	(F_CPU / 64.0f)
#define SECONDS_PER_TICK	(64.0f / F_CPU)

#define U_TICKS_PER_SECOND	(F_CPU / 64)

uint32_t
snek_ticks(void);

char
snek_uart_getch(void);

void
_snek_uart_puts(CONST char *string);

void
snek_uart_putch(char c);

void
snek_uart_wait_queued(char c, uint32_t ticks);

#define snek_uart_puts(string) ({ static CONST char __string__[] = (string); _snek_uart_puts(__string__); })

#define SNEK_IO_PUTS(s) snek_uart_puts(s)
#define SNEK_IO_LINEBUF 80
#define SNEK_IO_PUTC(c) snek_uart_putch(c)
#define SNEK_IO_GETC(s) snek_uart_getch()

extern FILE snek_duino_file;

#endif /* _SNEK_DUINO_H_ */
