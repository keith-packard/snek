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

#include "newt.h"

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
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
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

/*
 * Receive a character from the UART Rx.
 *
 * This features a simple line-editor that allows to delete and
 * re-edit the characters entered, until either CR or NL is entered.
 * Printable characters entered will be echoed using uart_putchar().
 *
 * Editing characters:
 *
 * . \b (BS) or \177 (DEL) delete the previous character
 * . ^u kills the entire input buffer
 * . ^w deletes the previous word
 * . ^r sends a CR, and then reprints the buffer
 * . \t will be replaced by a single space
 *
 * All other control characters will be ignored.
 *
 * The internal line buffer is RX_BUFSIZE (80) characters long, which
 * includes the terminating \n (but no terminating \0).  If the buffer
 * is full (i. e., at RX_BUFSIZE-1 characters in order to keep space for
 * the trailing \n), any further input attempts will send a \a to
 * uart_putchar() (BEL character), although line editing is still
 * allowed.
 *
 * Input errors while talking to the UART will cause an immediate
 * return of -1 (error indication).  Notably, this will be caused by a
 * framing error (e. g. serial line "break" condition), by an input
 * overrun, and by a parity error (if parity was enabled and automatic
 * parity recognition is supported by hardware).
 *
 * Successive calls to uart_getchar() will be satisfied from the
 * internal buffer until that buffer is emptied again.
 */

#define RX_BUFSIZE	64

newt_poly_t
newt_builtin_led_on(void)
{
	int ret = (PORTB >> PB5) & 1;
	DDRB |= (1 << PB5);
	PORTB |= (1 << PB5);
	return newt_float_to_poly(ret);
}

newt_poly_t
newt_builtin_led_off(void)
{
	int ret = (PORTB >> PB5) & 1;
	PORTB &= ~(1 << PB5);
	return newt_float_to_poly(ret);
}

newt_poly_t
newt_builtin_time_sleep(newt_poly_t a)
{
	if (newt_poly_type(a) == newt_float) {
		int cs = newt_poly_to_float(a) * 100.0f;
		while (cs--)
			_delay_ms(10);
	}
	return NEWT_ONE;
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
		for (cp = b;;)
		{
			loop_until_bit_is_set(UCSR0A, RXC0);
			if (UCSR0A & _BV(FE0))
				return _FDEV_EOF;
			if (UCSR0A & _BV(DOR0))
				return _FDEV_ERR;
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

			if ((c >= (uint8_t)' ' && c <= (uint8_t)'\x7e') ||
			    c >= (uint8_t)'\xa0')
			{
				if (cp == b + RX_BUFSIZE - 1)
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
			case 'c' & 0x1f:
				return -1;

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

			case 'r' & 0x1f:
				uart_putchar('\r', stream);
				for (cp2 = b; cp2 < cp; cp2++)
					uart_putchar(*cp2, stream);
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

			case 'w' & 0x1f:
				while (cp > b && cp[-1] != ' ')
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
	newt_print_vals = true;
	newt_parse();
}
