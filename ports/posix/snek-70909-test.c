/*
 * Copyright © 2025 Keith Packard <keithp@keithp.com>
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

#include "snek-70909.h"
#include <errno.h>

#define DEVICE "/dev/ttyUSB0"

const char anim[] = "\\|/-";

int main(void)
{
	lego_buf_t	l;
	int		s, i;
	int		t;

	if (lego_start(&l, DEVICE) < 0) {
		perror(DEVICE);
		return 1;
	}

	lego_ports_on(&l, 1 << 0);
	lego_port_on_cycle(&l, 0, 1);
	lego_port_off_cycle(&l, 0, 1);
	lego_ports_off(&l, 1 << 0);

	s = 0;
	for (t = 0; t < 1000; t++) {
		putchar(anim[s]);
		s = (s + 1) % (sizeof(anim) - 1);
		printf(":");
		for (i = 0; i < 8; i++) {
			lego_sensor_t value = lego_sensor(&l, i);
			printf (" [%d] %d.%d", i, value.value, value.status);
		}
		printf("\r"); fflush(stdout);
		usleep(1000 * 100);
	}
	lego_stop(&l);
	return 0;
}
