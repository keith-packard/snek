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

#include "snek.h"

#include <avr/io.h>
#include <util/delay.h>

/* UART baud rate */
#define UART_BAUD  9600
#define UART_BAUD_SCALE	(((F_CPU / (UART_BAUD * 16UL))) - 1)

#define NUM_PIN	21

static uint8_t	power_pin;
static uint8_t	dir_pin;
static uint8_t	input_pin;
static uint8_t	power[NUM_PIN];
static uint32_t	on_pins;

static void
uart_init(void)
{
	UBRR0H = (uint8_t) (UART_BAUD_SCALE >> 8);
	UBRR0L = (uint8_t) (UART_BAUD_SCALE);
	UCSR0A = ((1 << TXC0) |
		  (0 << U2X0) |
		  (0 << MPCM0));
	UCSR0B = ((0 << RXCIE0) |
		  (0 << TXCIE0) |
		  (0 << UDRIE0) |
		  (1 << RXEN0) |
		  (1 << TXEN0) |
		  (0 << UCSZ02) |
		  (0 << TXB80));
	UCSR0C = ((0 << UMSEL01) |
		  (0 << UMSEL00) |
		  (0 << UPM01) |
		  (0 << UPM00) |
		  (0 << USBS0) |
		  (1 << UCSZ00) |
		  (1 << UCSZ01) |
		  (0 << UCPOL0));
}

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

	memset(power, 0xff, NUM_PIN);
}

/*
 * Send character c down the UART Tx, wait until tx holding register
 * is empty.
 */
static int
uart_putchar(char c, FILE *stream)
{
	if (c == '\n')
		uart_putchar('\r', stream);
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
	return 0;
}

#define RX_BUFSIZE	128

static int
uart_getchar(FILE *stream)
{
	uint8_t c;
	char *cp;
	static char b[RX_BUFSIZE];
	static char *rxp;

	if (rxp == 0) {
		uart_putchar('>', stream);
		uart_putchar(' ', stream);
		cp = b;
		for (;;) {
			while ((UCSR0A & (1 << RXC0)) == 0);
			c = UDR0;
			/* behaviour similar to Unix stty ICRNL */
			if (c == '\r')
				c = '\n';
			if (c == '\n')
			{
				*cp = c;
				uart_putchar(c, stream);
				rxp = b;
				break;
			}

			switch (c)
			{
			case 'h' & 0x1f:
			case 0x7f:
				if (cp > b)
				{
					uart_putchar('\b', stream);
					uart_putchar(' ', stream);
					uart_putchar('\b', stream);
					cp--;
				}
				break;

			case 'u' & 0x1f:
				while (cp > b)
				{
					uart_putchar('\b', stream);
					uart_putchar(' ', stream);
					uart_putchar('\b', stream);
					cp--;
				}
				break;
			case '\t':
				c = ' ';
			default:
				if (c >= (uint8_t)' ') {
					if (cp >= b + RX_BUFSIZE - 1)
						uart_putchar('\a', stream);
					else
					{
						*cp++ = c;
						uart_putchar(c, stream);
					}
					continue;
				}
			}
		}
	}

	c = *rxp++;
	if (c == '\n')
		rxp = 0;

	return c;
}

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int
main (void)
{
	uart_init();
	port_init();
	stderr = stdout = stdin = &uart_str;
	fprintf(stdout, "Welcome to Snek\n");
	snek_print_vals = true;
	for (;;)
		snek_parse();
}

static volatile uint8_t *
ddr_reg(snek_soffset_t pin)
{
	if (pin < 8)
		return &DDRD;
	if (pin < 14)
		return &DDRB;
	return &DDRC;
}

static volatile uint8_t *
pin_reg(snek_soffset_t pin)
{
	if (pin < 8)
		return &PIND;
	if (pin < 14)
		return &PINB;
	return &PINC;
}

static volatile uint8_t *
port_reg(snek_soffset_t pin)
{
	if (pin < 8)
		return &PORTD;
	if (pin < 14)
		return &PORTB;
	return &PORTC;
}

static uint8_t
bit(snek_soffset_t pin)
{
	if (pin < 8)
		;
	else if (pin < 14)
		pin = pin - 8;
	else
		pin = pin - 14;
	return 1 << pin;
}

static bool
has_pwm(snek_soffset_t p)
{
	return ((p) == 3 || (p) == 5 || (p) == 6 || (p) == 9 || (p) == 10 || (p) == 11);
}

static volatile uint8_t *
ocr_reg(snek_soffset_t pin)
{
	switch(pin) {
	case 3:
		return &OCR2B;
	case 5:
		return &OCR0B;
	case 6:
		return &OCR0A;
	case 9:
		return &OCR1AL;
	case 10:
		return &OCR1BL;
	case 11:
		return &OCR2A;
	default:
		return NULL;
	}
}

static volatile uint8_t *
tcc_reg(snek_soffset_t pin)
{
	switch(pin) {
	case 3:
		return &TCCR2A;
	case 5:
		return &TCCR0A;
	case 6:
		return &TCCR0A;
	case 9:
		return &TCCR1A;
	case 10:
		return &TCCR1A;
	case 11:
		return &TCCR2A;
	default:
		return NULL;
	}
}

static uint8_t
tcc_val(snek_soffset_t pin)
{
	switch (pin) {
	case 3:
		return 1 << COM2B1;
	case 5:
		return 1 << COM0B1;
	case 6:
		return 1 << COM0A1;
	case 9:
		return 1 << COM1A1;
	case 10:
		return 1 << COM1B1;
	case 11:
		return 1 << COM2A1;
	default:
		return 0;
	}
}

static void
set_dir(snek_soffset_t pin, uint8_t d)
{
	volatile uint8_t *r = ddr_reg(pin);
	volatile uint8_t *p = port_reg(pin);
	uint8_t	b = bit(pin);

	if (d) {
		*r |= b;
		*p &= ~b;
	} else {
		*r &= ~b;
		*p |= b;
	}
}

snek_poly_t
snek_builtin_talkto(snek_poly_t a)
{
	snek_list_t *l;
	switch (snek_poly_type(a)) {
	case snek_float:
		power_pin = dir_pin = snek_poly_get_soffset(a);
		break;
	case snek_list:
		l = snek_poly_to_list(a);
		power_pin = snek_poly_get_soffset(snek_list_get(l, 0));
		dir_pin = snek_poly_get_soffset(snek_list_get(l, 1));
		break;
	default:
		break;
	}
	if (power_pin >= NUM_PIN)
		power_pin = 0;
	if (dir_pin >= NUM_PIN)
		dir_pin = 0;
	set_dir(power_pin, 1);
	set_dir(dir_pin, 1);
	return a;
}

snek_poly_t
snek_builtin_listento(snek_poly_t a)
{
	input_pin = snek_poly_get_soffset(a);
	set_dir(input_pin, 0);
	return a;
}

static bool
is_on(uint8_t pin)
{
	return on_pins >> pin & 1;
}

static void
set_on(uint8_t pin)
{
	on_pins |= ((uint32_t) 1) << pin;
}

static void
set_off(uint8_t pin)
{
	on_pins &= ~((uint32_t) 1) << pin;
}

static snek_poly_t
set_out(uint8_t pin)
{
	uint8_t	p = 0;

	if (is_on(pin))
		p = power[pin];

	if (has_pwm(pin)) {
		if (0 < p && p < 255) {
			*ocr_reg(pin) = p;
			*tcc_reg(pin) |= tcc_val(pin);
			return SNEK_ZERO;
		}
		*tcc_reg(pin) &= ~tcc_val(pin);
	}
	if (p)
		*port_reg(pin) |= bit(pin);
	else
		*port_reg(pin) &= ~bit(pin);
	return SNEK_ZERO;
}

snek_poly_t
snek_builtin_setpower(snek_poly_t a)
{
	power[power_pin] = (uint8_t) (snek_poly_get_float(a) * 255.0f + 0.5f);
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
	snek_builtin_off();
	return a;
}

#define analog_reference 1

snek_poly_t
snek_builtin_read(void)
{
	if (input_pin >= 14) {
		uint8_t pin = input_pin - 14;
		ADMUX = (analog_reference << 6) | (pin & 7);
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC))
			;
		uint8_t low = ADCL;
		uint8_t high = ADCH;
		float value = ((uint16_t) high << 8 | low) / 1023.0;

		return snek_float_to_poly(value);
	} else {
		return snek_bool_to_poly(*pin_reg(input_pin) & bit(input_pin));
	}
}

snek_poly_t
snek_builtin_stopall(void)
{
	snek_soffset_t p;
	for (p = 0; p < NUM_PIN; p++)
		if (on_pins & ((uint32_t) 1 << p)) {
			set_off(p);
			set_out(p);
		}
	return SNEK_ZERO;
}

snek_poly_t
snek_builtin_time_sleep(snek_poly_t a)
{
	snek_soffset_t o = snek_poly_get_float(a) * 100.0f;
	while (o-- >= 0)
		_delay_ms(10);
	return SNEK_ONE;
}

