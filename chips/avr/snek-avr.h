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

#ifndef _SNEK_AVR_H_
#define _SNEK_AVR_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <math.h>

#define sqrtf snek_sqrtf

static inline float
sqrtf(float x) {
	return powf(x, 0.5f);
}

#define SNEK_DEBUG	0
float atoff(const char *);
#define strtof(s, n) atoff(s)
#define SNEK_POOL		1024
#define SNEK_MAX_TOKEN		63
#define VALUE_STACK_SIZE	16
#define PARSE_STACK_SIZE	128
#define SNEK_STACK		32

#ifndef CONST
#define CONST const __flash
#endif

/* no sense linking both functions */
#define memcpy(a,b,c) memmove(a,b,c)

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
		printf_P((const char *) (uintptr_t) __fmt__, ## args);		\
	})

#define sprintf_const(dst, fmt, args...) ({			\
		static CONST char __fmt__[] = (fmt);	\
		sprintf_P(dst, (const char *) (uintptr_t) __fmt__, ##args); \
	})

int
strfromg(char *dst0, float val);

//#define strfromf_const(dst, len, fmt, val) sprintf_const(dst, fmt, val)
//#define strfromf(dst, len, fmt, val) sprintf(dst, fmt, val)
#define strfromf(dst, len, fmt, val) strfromg(dst, val)

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
#define snek_builtin_names_len(a) strnlen_P((const char *) (uintptr_t) a, SNEK_BUILTIN_NAMES_MAX_LEN+1)

int
snek_eeprom_getchar(FILE *stream);

#define SNEK_IO_LINEBUF 80

extern FILE snek_avr_file;

#endif /* _SNEK_AVR_H_ */
