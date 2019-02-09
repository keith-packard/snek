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

/* UART baud rate */
#define UART_BAUD  9600
#define UART_BAUD_SCALE	(((F_CPU / (UART_BAUD * 16UL))) - 1)

/*
 * Send character c down the UART Tx, wait until tx holding register
 * is empty.
 */
int
snek_uart_putchar(char c, FILE *stream)
{
	(void) stream;
	if (c == '\n')
		snek_uart_putchar('\r', NULL);
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
	return 0;
}

#define RX_BUFSIZE	128

static void
snek_uart_backspace(void)
{
	snek_uart_putchar('\b', NULL);
	snek_uart_putchar(' ', NULL);
	snek_uart_putchar('\b', NULL);
}

int
snek_uart_getchar(FILE *stream)
{
	(void) stream;

	uint8_t c;
	char *cp;
	static char b[RX_BUFSIZE];
	static char *rxp;

	if (rxp == 0) {
		snek_uart_putchar('>', NULL);
		snek_uart_putchar(' ', NULL);
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
				snek_uart_putchar(c, NULL);
				rxp = b;
				break;
			}

			switch (c)
			{
			case 'h' & 0x1f:
			case 0x7f:
				if (cp > b) {
					snek_uart_backspace();
					cp--;
				}
				break;

			case 'u' & 0x1f:
				while (cp > b) {
					snek_uart_backspace();
					cp--;
				}
				break;
			case '\t':
				c = ' ';
			default:
				if (c >= (uint8_t)' ') {
					if (cp >= b + RX_BUFSIZE - 1)
						snek_uart_putchar('\a', NULL);
					else
					{
						*cp++ = c;
						snek_uart_putchar(c, NULL);
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

FILE snek_uart_str = FDEV_SETUP_STREAM(snek_uart_putchar, snek_uart_getchar, _FDEV_SETUP_RW);

void
snek_uart_init(void)
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
	stderr = stdout = stdin = &snek_uart_str;
}

