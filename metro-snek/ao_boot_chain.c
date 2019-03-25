/*
 * Copyright © 2013 Keith Packard <keithp@keithp.com>
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

#include <ao.h>
#include <ao_boot.h>

void
ao_boot_chain(uint32_t *base)
{
	uint32_t	sp;
	uint32_t	pc;

	sp = base[0];
	pc = base[1];
	if (0x08000100 <= pc && pc <= 0x08200000 && (pc & 1) == 1) {
		asm ("mov sp, %0" : : "r" (sp));
		asm ("mov lr, %0" : : "r" (pc));
		asm ("bx lr");
	}
}

#define AO_BOOT_SIGNAL	0x5a5aa5a5
#define AO_BOOT_CHECK	0xc3c33c3c

struct ao_boot {
	uint32_t	*base;
	uint32_t	signal;
	uint32_t	check;
};

static struct ao_boot __attribute__ ((section(".boot"))) ao_boot;

int
ao_boot_check_chain(void)
{
	if (ao_boot.signal == AO_BOOT_SIGNAL && ao_boot.check == AO_BOOT_CHECK) {
		ao_boot.signal = 0;
		ao_boot.check = 0;
		if (ao_boot.base == AO_BOOT_FORCE_LOADER)
			return 0;
		ao_boot_chain(ao_boot.base);
	}
	return 1;
}

void
ao_boot_reboot(uint32_t *base)
{
	ao_boot.base = base;
	ao_boot.signal = AO_BOOT_SIGNAL;
	ao_boot.check = AO_BOOT_CHECK;
	ao_arch_reboot();
}
