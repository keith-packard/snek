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

#include "snek.h"

#define NUM_PIN		32
#define A0_INDEX	24

static uint8_t	power_pin;
static uint8_t	dir_pin;
static uint8_t	power[NUM_PIN];
static uint32_t	on_pins;

#define SNEK_PULL_NONE	0
#define SNEK_PULL_UP	1

/* digital pins all use PULL_UP by default */
static uint32_t pull_pins = 0x00ffffff;

static void
port_init(void)
{
	/* Enable ADC */
	ADCSRA = ((1 << ADPS2) |
		  (1 << ADPS1) |
		  (1 << ADPS0) |
		  (1 << ADEN));

	/* Timer 0 */
	TCCR0A = ((1 << WGM01) |
		  (1 << WGM00));

	/* / 64 */
	TCCR0B = ((0 << CS02) |
		  (1 << CS01) |
		  (1 << CS00));

	/* enable interrupt */
	TIMSK0 = (1 << TOIE0);

	/* Timer 1 */
	TCCR1B = ((0 << CS12) |
		  (1 << CS11) |
		  (1 << CS10));

	TCCR1A = ((0 << WGM12) |
		  (0 << WGM11) |
		  (1 << WGM10));

	OCR1AH = 0;
	OCR1BH = 0;

	/* Timer 2 */
	TCCR2B = ((1 << CS22) |
		  (0 << CS21) |
		  (0 << CS20));

	TCCR2A = ((1 << WGM20));

	/* Timer 3 */
	TCCR3B = ((1 << CS32) |
		  (0 << CS31) |
		  (0 << CS30));

	TCCR2A = ((1 << WGM30));

	memset(power, 0xff, NUM_PIN);
}

FILE snek_duino_file = FDEV_SETUP_STREAM(snek_uart_putchar, snek_eeprom_getchar, _FDEV_SETUP_RW);

#include <avr/wdt.h>

int __attribute__((OS_main))
main (void)
{
	MCUSR = 0;
	wdt_disable();
	stderr = stdout = stdin = &snek_duino_file;
	snek_uart_init();
	port_init();
	snek_init();
	for (;;)
		snek_parse();
}

snek_poly_t
snek_builtin_reset(void)
{
	wdt_enable(WDTO_15MS);
	for(;;);
	return SNEK_NULL;
}

static volatile uint8_t *
ddr_reg(uint8_t pin)
{
	if (pin < 8)
		return &DDRB;
	if (pin < 16)
		return &DDRD;
	if (pin < 24)
		return &DDRC;
	return &DDRA;
}

static volatile uint8_t *
pin_reg(uint8_t pin)
{
	if (pin < 8)
		return &PINB;
	if (pin < 16)
		return &PIND;
	return &PINC;
}

static volatile uint8_t *
port_reg(uint8_t pin)
{
	if (pin < 8)
		return &PORTB;
	if (pin < 16)
		return &PORTD;
	if (pin < 24)
		return &PORTC;
	return &PORTA;
}

static uint8_t
bit(uint8_t pin)
{
	return 1 << (pin & 0x7);
}

static bool
is_pull(uint8_t pin)
{
	return (pull_pins >> pin) & 1;
}

static bool
has_pwm(uint8_t p)
{
	const uint32_t pwm_pins = ((1 << 3) | (1 << 4) | (1 << 6)
				   | (1 << 7) | (1 << 12) | (1 << 13)
				   | (1 << 14) | (1 << 15));

	p &= 31;

	return pwm_pins & (1 << p);
}

static volatile uint8_t * const PROGMEM ocr8_reg_addrs[] = {
	[3] = &OCR0A,
	[4] = &OCR0B,
	[14] = &OCR2B,
	[15] = &OCR2A,
};

static volatile uint16_t * const PROGMEM ocr16_reg_addrs[] = {
	[6] = &OCR3A,
	[7] = &OCR3B,
	[12] = &OCR1B,
	[13] = &OCR1A,
};

static volatile uint8_t *
ocr8_reg(uint8_t pin) {
	return (volatile uint8_t *) pgm_read_word(&ocr8_reg_addrs[pin]);
}

static volatile uint16_t *
ocr16_reg(uint8_t pin) {
	return (volatile uint16_t *) pgm_read_word(&ocr16_reg_addrs[pin]);
}

static volatile uint8_t * const PROGMEM tcc_reg_addrs[] = {
	[3] = &TCCR0A,
	[4] = &TCCR0B,
	[6] = &TCCR3A,
	[7] = &TCCR3B,
	[12] = &TCCR1B,
	[13] = &TCCR1A,
	[14] = &TCCR2B,
	[15] = &TCCR2A,
};

static volatile uint8_t *
tcc_reg(uint8_t pin) {
	return (volatile uint8_t *) pgm_read_word(&tcc_reg_addrs[pin]);
}

static uint8_t const PROGMEM tcc_val_addrs[] = {
	[3] = 1 << COM0A1,
	[4] = 1 << COM0B1,
	[6] = 1 << COM3A1,
	[7] = 1 << COM3B1,
	[12] = 1 << COM1B1,
	[13] = 1 << COM1A1,
	[14] = 1 << COM2B1,
	[15] = 1 << COM2A1,
};

static uint8_t
tcc_val(uint8_t pin)
{
	return (uint8_t) pgm_read_byte(&tcc_val_addrs[pin]);
}

static void
set_dir(uint8_t pin, uint8_t d)
{
	volatile uint8_t *r = ddr_reg(pin);
	volatile uint8_t *p = port_reg(pin);
	uint8_t	b = bit(pin);

	if (d) {
		*r |= b;
	} else {
		*r &= ~b;
		if (is_pull(pin))
			*p |= b;
		else
			*p &= ~b;
	}
}

static uint8_t
snek_poly_get_pin(snek_poly_t a)
{
	snek_soffset_t p = snek_poly_get_soffset(a);
	if (p < 0 || NUM_PIN <= p)
		snek_error_value(a);
	return p;
}

snek_poly_t
snek_builtin_talkto(snek_poly_t a)
{
	snek_list_t *l;
	uint8_t p, d;

	if (snek_poly_type(a) == snek_list) {
		l = snek_poly_to_list(a);
		p = snek_poly_get_pin(snek_list_get(l, SNEK_ZERO, true));
		d = snek_poly_get_pin(snek_list_get(l, SNEK_ONE, true));
	} else {
		p = d = snek_poly_get_pin(a);
	}
	if (!snek_abort) {
		set_dir(p, 1);
		set_dir(d, 1);
		power_pin = p;
		dir_pin = d;
	}
	return SNEK_NULL;
}

static bool
is_on(uint8_t pin)
{
	return (on_pins >> pin) & 1;
}

static void
set_on(uint8_t pin)
{
	on_pins |= ((uint32_t) 1) << pin;
}

static void
set_off(uint8_t pin)
{
	on_pins &= ~(((uint32_t) 1) << pin);
}

static snek_poly_t
set_out(uint8_t pin)
{
	uint8_t	p = 0;

	if (is_on(pin))
		p = power[pin];

	if (has_pwm(pin)) {
		if (0 < p && p < 255) {
			if (ocr8_reg(pin))
				*ocr8_reg(pin) = p;
			else
				*ocr16_reg(pin) = (((uint16_t) p) << 8) | p;
			*tcc_reg(pin) |= tcc_val(pin);
			return SNEK_NULL;
		}
		*tcc_reg(pin) &= ~tcc_val(pin);
	}
	if (p)
		*port_reg(pin) |= bit(pin);
	else
		*port_reg(pin) &= ~bit(pin);
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_setpower(snek_poly_t a)
{
	float p = snek_poly_get_float(a);
	if (p < 0.0f) p = 0.0f;
	if (p > 1.0f) p = 1.0f;
	power[power_pin] = (uint8_t) (p * 255.0f + 0.5f);
	return set_out(power_pin);
}

snek_poly_t
snek_builtin_setleft(void)
{
	set_on(dir_pin);
	return set_out(dir_pin);
}

snek_poly_t
snek_builtin_setright(void)
{
	set_off(dir_pin);
	return set_out(dir_pin);
}

snek_poly_t
snek_builtin_on(void)
{
	set_on(power_pin);
	return set_out(power_pin);
}

snek_poly_t
snek_builtin_off(void)
{
	set_off(power_pin);
	return set_out(power_pin);
}

snek_poly_t
snek_builtin_onfor(snek_poly_t a)
{
	snek_builtin_on();
	snek_builtin_time_sleep(a);
	return snek_builtin_off();
}

snek_poly_t
snek_builtin_pullnone(snek_poly_t a)
{
	uint8_t p = snek_poly_get_pin(a);
	if (!snek_abort)
		pull_pins &= ~(1 << p);
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_pullup(snek_poly_t a)
{
	uint8_t p = snek_poly_get_pin(a);
	if (!snek_abort)
		pull_pins |= 1 << p;
	return SNEK_NULL;
}

#define analog_reference 1

snek_poly_t
snek_builtin_read(snek_poly_t a)
{
	uint8_t p = snek_poly_get_pin(a);
	if (snek_abort)
		return SNEK_NULL;
	set_dir(p, 0);

	if (p >= A0_INDEX && !is_pull(p)) {
		uint8_t pin = p - A0_INDEX;
		ADMUX = (analog_reference << REFS0) | (pin & 7);
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC))
			;
		uint8_t low = ADCL;
		uint8_t high = ADCH;
		float value = ((uint16_t) high << 8 | low) / 1023.0;

		return snek_float_to_poly(value);
	} else {
		return snek_bool_to_poly(*pin_reg(p) & bit(p));
	}
}

snek_poly_t
snek_builtin_stopall(void)
{
	uint8_t p;
	for (p = 0; p < NUM_PIN; p++)
		if (on_pins & ((uint32_t) 1 << p)) {
			set_off(p);
			set_out(p);
		}
	return SNEK_NULL;
}

static uint32_t random_next;

snek_poly_t
snek_builtin_random_seed(snek_poly_t a)
{
	random_next = a.u;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_random_randrange(snek_poly_t a)
{
	random_next = random_next * 1103515245L + 12345L;
	return snek_float_to_poly(random_next % (uint32_t) snek_poly_get_float(a));
}
