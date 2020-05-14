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

#ifndef _AO_SNEK_H_
#define _AO_SNEK_H_

#include <ao.h>

#define RX_LINEBUF	132

#define SNEK_GETC()		getc(stdin)
#define SNEK_POOL		(16 * 1024)

#define SNEK_IO_GETC(file)	ao_usb_getc()
#define SNEK_IO_WAITING(file)	ao_usb_waiting()

void
ao_snek_set_pwm(void *gpio, uint8_t pin, void *timer, uint8_t c, uint16_t value);

void
ao_snek_clr_pwm(void *gpio, uint8_t pin);

#ifdef AO_SNEK_PWM_RAMP_STEP
#define AO_TIMER_HOOK		ao_snek_step_pwm()
void
ao_snek_step_pwm(void);
#endif

int
ao_snek_getc(FILE *stream);

void
snek(void);

extern bool snek_eof;

#define SNEK_PIN_PULL_DOWN	0x01
#define SNEK_PIN_PULL_UP	0x02
#define SNEK_PIN_RAMP_PWM	0x04

void
snek_eeprom_load(void);

int
snek_eeprom_getchar(FILE *stream);

void
ao_snek_set_adc(void *gpio, uint8_t pin);

void
ao_snek_clr_adc(void *gpio, uint8_t pin);

uint16_t
ao_snek_get_adc(uint8_t adc);

void
ao_snek_port_init(void);

void
ao_snek(void);

void
ao_snek_reset(void);

void
ao_snek_running(bool running);

#ifdef AO_LED_TX
#define SNEK_CODE_HOOK_START	ao_snek_running(true);
#define SNEK_CODE_HOOK_STOP	ao_snek_running(false);
#endif

struct snek_neopixel {
	union {
		struct {
			uint8_t	_extra, b, r, g;
		};
		uint32_t	p;
	};
};

extern struct snek_neopixel	*snek_neopixels;

#define SNEK_MEM_CACHE_NUM	1
#define SNEK_MEM_CACHE_0	snek_neopixels

void
ao_snek_neopixel_write(void *port, uint8_t pin, int npixel, const struct snek_neopixel *pixels);

void
ao_snek_apa102_write(void *gpio_d, uint8_t pin_d,
		     void *gpio_c, uint8_t pin_c,
		     int npixel,
		     struct snek_neopixel *pixels);

#endif /* _AO_SNEK_H_ */
