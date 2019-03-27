/*
 * Copyright © 2019 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include "ao.h"
#include "ao-snek.h"
#include "snek.h"
#include <setjmp.h>

static jmp_buf	snek_reset_buf;

void
ao_snek(void)
{
	setjmp(snek_reset_buf);
	ao_snek_port_init();
	snek_builtin_eeprom_load();
	printf("Welcome to snek " SNEK_VERSION "\n");
	fflush(stdout);
	ao_snek_running(false);
	for (;;) {
		snek_interactive = true;
		snek_parse();
	}
}

extern char __snek_data_start__, __snek_data_end__;
extern char __snek_bss_start__, __snek_bss_end__;
extern char __text_start__, __text_end__;
extern char __data_start__, __data_end__;
extern char __bss_start__, __bss_end__;

snek_poly_t
snek_builtin_reset(void)
{
	/* reset data */
	memcpy(&__snek_data_start__,
	       &__text_end__ + (&__snek_data_start__ - &__data_start__),
	       &__snek_data_end__ - &__snek_data_start__);

	/* reset bss */
	memset(&__snek_bss_start__, '\0', &__snek_bss_end__ - &__snek_bss_start__);

	/* and off we go! */
	longjmp(snek_reset_buf, 1);
	return SNEK_NULL;
}
