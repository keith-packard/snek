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

/*
 * Send character c down the UART Tx, wait until tx holding register
 * is empty.
 */
int
uart_putchar(char c, FILE *stream)
{
	if (c == '\n')
		uart_putchar('\r', stream);
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
	return 0;
}

#define RX_BUFSIZE	64

snek_poly_t
snek_builtin_led_on(void)
{
	int ret = (PORTB >> PB5) & 1;
	DDRB |= (1 << PB5);
	PORTB |= (1 << PB5);
	return snek_float_to_poly(ret);
}

snek_poly_t
snek_builtin_led_off(void)
{
	int ret = (PORTB >> PB5) & 1;
	PORTB &= ~(1 << PB5);
	return snek_float_to_poly(ret);
}

snek_poly_t
snek_builtin_time_sleep(snek_poly_t a)
{
	if (snek_poly_type(a) == snek_float) {
		int cs = snek_poly_to_float(a) * 100.0f;
		while (cs--)
			_delay_ms(10);
	}
	return SNEK_ONE;
}

int
uart_getchar(FILE *stream)
{
	uint8_t c;
	char *cp, *cp2;
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
			else if (c == '\t')
				c = ' ';

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

			switch (c)
			{
			case '\b':
			case '\x7f':
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
			}
		}
	}

	c = *rxp++;
	if (c == '\n')
		rxp = 0;

	return c;
}

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int main (void)
{
	uart_init();
	stderr = stdout = stdin = &uart_str;
	fprintf(stdout, "Welcome to Snek\n");
	snek_print_vals = true;
	snek_parse();
}
