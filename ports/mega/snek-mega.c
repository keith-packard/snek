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

#define NUM_PIN	70
#define A0	54

static uint8_t	power_pin;
static uint8_t	dir_pin;
static uint8_t	power[NUM_PIN];
static uint8_t	on_pins[(NUM_PIN + 7) >> 3];

static uint8_t	pull_pins[(NUM_PIN + 7) >> 3];

static inline uint8_t pin_byte(uint8_t p)
{
	return p >> 3;
}

static inline uint8_t pin_bit(uint8_t p)
{
	return (p & 7);
}

#define clockCyclesPerMicrosecond	(F_CPU / 1000000L)
#define clockCyclesToMicroseconds(a)	((a) / clockCyclesPerMicrosecond)

// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile uint32_t timer0_millis = 0;
static uint8_t timer0_fract = 0;

ISR(TIMER0_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	uint32_t m = timer0_millis;
	uint8_t f = timer0_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}
	timer0_fract = f;
	timer0_millis = m;
}

static void
port_init(void)
{
	uint8_t p;

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
		  (1 << CS00) |
		  (0 << WGM02));

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
	TCCR3B = ((0 << CS12) |
		  (1 << CS11) |
		  (1 << CS10));

	TCCR3A = ((0 << WGM12) |
		  (0 << WGM11) |
		  (1 << WGM10));

	OCR3AH = 0;
	OCR3BH = 0;

	/* Timer 4 */
	TCCR4B = ((0 << CS12) |
		  (1 << CS11) |
		  (1 << CS10));

	TCCR4A = ((0 << WGM12) |
		  (0 << WGM11) |
		  (1 << WGM10));

	OCR4AH = 0;
	OCR4BH = 0;

	/* Timer 5 */
	TCCR5B = ((0 << CS12) |
		  (1 << CS11) |
		  (1 << CS10));

	TCCR5A = ((0 << WGM12) |
		  (0 << WGM11) |
		  (1 << WGM10));

	OCR5AH = 0;
	OCR5BH = 0;

	memset(power, 0xff, sizeof(power));
	memset(pull_pins, 0x00, sizeof(pull_pins));
	for (p = 0; p < A0; p++)
		pull_pins[pin_byte(p)] |= (1 << pin_bit(p));
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

#define PA 1
#define PB 2
#define PC 3
#define PD 4
#define PE 5
#define PF 6
#define PG 7
#define PH 8
#define PJ 10
#define PK 11
#define PL 12

#define NOT_A_PIN 0
#define NOT_A_PORT 0

typedef volatile uint8_t vuint8_t;

static vuint8_t * PROGMEM const port_to_mode_PGM[] = {
	NOT_A_PORT,
	&DDRA,
	&DDRB,
	&DDRC,
	&DDRD,
	&DDRE,
	&DDRF,
	&DDRG,
	&DDRH,
	NOT_A_PORT,
	&DDRJ,
	&DDRK,
	&DDRL,
};

static vuint8_t * PROGMEM const port_to_output_PGM[] = {
	NOT_A_PORT,
	&PORTA,
	&PORTB,
	&PORTC,
	&PORTD,
	&PORTE,
	&PORTF,
	&PORTG,
	&PORTH,
	NOT_A_PORT,
	&PORTJ,
	&PORTK,
	&PORTL,
};

static vuint8_t * PROGMEM const port_to_input_PGM[] = {
	NOT_A_PIN,
	&PINA,
	&PINB,
	&PINC,
	&PIND,
	&PINE,
	&PINF,
	&PING,
	&PINH,
	NOT_A_PIN,
	&PINJ,
	&PINK,
	&PINL,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	// PORTLIST
	// -------------------------------------------
	PE	, // PE 0 ** 0 ** USART0_RX
	PE	, // PE 1 ** 1 ** USART0_TX
	PE	, // PE 4 ** 2 ** PWM2
	PE	, // PE 5 ** 3 ** PWM3
	PG	, // PG 5 ** 4 ** PWM4
	PE	, // PE 3 ** 5 ** PWM5
	PH	, // PH 3 ** 6 ** PWM6
	PH	, // PH 4 ** 7 ** PWM7
	PH	, // PH 5 ** 8 ** PWM8
	PH	, // PH 6 ** 9 ** PWM9
	PB	, // PB 4 ** 10 ** PWM10
	PB	, // PB 5 ** 11 ** PWM11
	PB	, // PB 6 ** 12 ** PWM12
	PB	, // PB 7 ** 13 ** PWM13
	PJ	, // PJ 1 ** 14 ** USART3_TX
	PJ	, // PJ 0 ** 15 ** USART3_RX
	PH	, // PH 1 ** 16 ** USART2_TX
	PH	, // PH 0 ** 17 ** USART2_RX
	PD	, // PD 3 ** 18 ** USART1_TX
	PD	, // PD 2 ** 19 ** USART1_RX
	PD	, // PD 1 ** 20 ** I2C_SDA
	PD	, // PD 0 ** 21 ** I2C_SCL
	PA	, // PA 0 ** 22 ** D22
	PA	, // PA 1 ** 23 ** D23
	PA	, // PA 2 ** 24 ** D24
	PA	, // PA 3 ** 25 ** D25
	PA	, // PA 4 ** 26 ** D26
	PA	, // PA 5 ** 27 ** D27
	PA	, // PA 6 ** 28 ** D28
	PA	, // PA 7 ** 29 ** D29
	PC	, // PC 7 ** 30 ** D30
	PC	, // PC 6 ** 31 ** D31
	PC	, // PC 5 ** 32 ** D32
	PC	, // PC 4 ** 33 ** D33
	PC	, // PC 3 ** 34 ** D34
	PC	, // PC 2 ** 35 ** D35
	PC	, // PC 1 ** 36 ** D36
	PC	, // PC 0 ** 37 ** D37
	PD	, // PD 7 ** 38 ** D38
	PG	, // PG 2 ** 39 ** D39
	PG	, // PG 1 ** 40 ** D40
	PG	, // PG 0 ** 41 ** D41
	PL	, // PL 7 ** 42 ** D42
	PL	, // PL 6 ** 43 ** D43
	PL	, // PL 5 ** 44 ** D44
	PL	, // PL 4 ** 45 ** D45
	PL	, // PL 3 ** 46 ** D46
	PL	, // PL 2 ** 47 ** D47
	PL	, // PL 1 ** 48 ** D48
	PL	, // PL 0 ** 49 ** D49
	PB	, // PB 3 ** 50 ** SPI_MISO
	PB	, // PB 2 ** 51 ** SPI_MOSI
	PB	, // PB 1 ** 52 ** SPI_SCK
	PB	, // PB 0 ** 53 ** SPI_SS
	PF	, // PF 0 ** 54 ** A0
	PF	, // PF 1 ** 55 ** A1
	PF	, // PF 2 ** 56 ** A2
	PF	, // PF 3 ** 57 ** A3
	PF	, // PF 4 ** 58 ** A4
	PF	, // PF 5 ** 59 ** A5
	PF	, // PF 6 ** 60 ** A6
	PF	, // PF 7 ** 61 ** A7
	PK	, // PK 0 ** 62 ** A8
	PK	, // PK 1 ** 63 ** A9
	PK	, // PK 2 ** 64 ** A10
	PK	, // PK 3 ** 65 ** A11
	PK	, // PK 4 ** 66 ** A12
	PK	, // PK 5 ** 67 ** A13
	PK	, // PK 6 ** 68 ** A14
	PK	, // PK 7 ** 69 ** A15
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	// PIN IN PORT
	// -------------------------------------------
	_BV( 0 )	, // PE 0 ** 0 ** USART0_RX
	_BV( 1 )	, // PE 1 ** 1 ** USART0_TX
	_BV( 4 )	, // PE 4 ** 2 ** PWM2
	_BV( 5 )	, // PE 5 ** 3 ** PWM3
	_BV( 5 )	, // PG 5 ** 4 ** PWM4
	_BV( 3 )	, // PE 3 ** 5 ** PWM5
	_BV( 3 )	, // PH 3 ** 6 ** PWM6
	_BV( 4 )	, // PH 4 ** 7 ** PWM7
	_BV( 5 )	, // PH 5 ** 8 ** PWM8
	_BV( 6 )	, // PH 6 ** 9 ** PWM9
	_BV( 4 )	, // PB 4 ** 10 ** PWM10
	_BV( 5 )	, // PB 5 ** 11 ** PWM11
	_BV( 6 )	, // PB 6 ** 12 ** PWM12
	_BV( 7 )	, // PB 7 ** 13 ** PWM13
	_BV( 1 )	, // PJ 1 ** 14 ** USART3_TX
	_BV( 0 )	, // PJ 0 ** 15 ** USART3_RX
	_BV( 1 )	, // PH 1 ** 16 ** USART2_TX
	_BV( 0 )	, // PH 0 ** 17 ** USART2_RX
	_BV( 3 )	, // PD 3 ** 18 ** USART1_TX
	_BV( 2 )	, // PD 2 ** 19 ** USART1_RX
	_BV( 1 )	, // PD 1 ** 20 ** I2C_SDA
	_BV( 0 )	, // PD 0 ** 21 ** I2C_SCL
	_BV( 0 )	, // PA 0 ** 22 ** D22
	_BV( 1 )	, // PA 1 ** 23 ** D23
	_BV( 2 )	, // PA 2 ** 24 ** D24
	_BV( 3 )	, // PA 3 ** 25 ** D25
	_BV( 4 )	, // PA 4 ** 26 ** D26
	_BV( 5 )	, // PA 5 ** 27 ** D27
	_BV( 6 )	, // PA 6 ** 28 ** D28
	_BV( 7 )	, // PA 7 ** 29 ** D29
	_BV( 7 )	, // PC 7 ** 30 ** D30
	_BV( 6 )	, // PC 6 ** 31 ** D31
	_BV( 5 )	, // PC 5 ** 32 ** D32
	_BV( 4 )	, // PC 4 ** 33 ** D33
	_BV( 3 )	, // PC 3 ** 34 ** D34
	_BV( 2 )	, // PC 2 ** 35 ** D35
	_BV( 1 )	, // PC 1 ** 36 ** D36
	_BV( 0 )	, // PC 0 ** 37 ** D37
	_BV( 7 )	, // PD 7 ** 38 ** D38
	_BV( 2 )	, // PG 2 ** 39 ** D39
	_BV( 1 )	, // PG 1 ** 40 ** D40
	_BV( 0 )	, // PG 0 ** 41 ** D41
	_BV( 7 )	, // PL 7 ** 42 ** D42
	_BV( 6 )	, // PL 6 ** 43 ** D43
	_BV( 5 )	, // PL 5 ** 44 ** D44
	_BV( 4 )	, // PL 4 ** 45 ** D45
	_BV( 3 )	, // PL 3 ** 46 ** D46
	_BV( 2 )	, // PL 2 ** 47 ** D47
	_BV( 1 )	, // PL 1 ** 48 ** D48
	_BV( 0 )	, // PL 0 ** 49 ** D49
	_BV( 3 )	, // PB 3 ** 50 ** SPI_MISO
	_BV( 2 )	, // PB 2 ** 51 ** SPI_MOSI
	_BV( 1 )	, // PB 1 ** 52 ** SPI_SCK
	_BV( 0 )	, // PB 0 ** 53 ** SPI_SS
	_BV( 0 )	, // PF 0 ** 54 ** A0
	_BV( 1 )	, // PF 1 ** 55 ** A1
	_BV( 2 )	, // PF 2 ** 56 ** A2
	_BV( 3 )	, // PF 3 ** 57 ** A3
	_BV( 4 )	, // PF 4 ** 58 ** A4
	_BV( 5 )	, // PF 5 ** 59 ** A5
	_BV( 6 )	, // PF 6 ** 60 ** A6
	_BV( 7 )	, // PF 7 ** 61 ** A7
	_BV( 0 )	, // PK 0 ** 62 ** A8
	_BV( 1 )	, // PK 1 ** 63 ** A9
	_BV( 2 )	, // PK 2 ** 64 ** A10
	_BV( 3 )	, // PK 3 ** 65 ** A11
	_BV( 4 )	, // PK 4 ** 66 ** A12
	_BV( 5 )	, // PK 5 ** 67 ** A13
	_BV( 6 )	, // PK 6 ** 68 ** A14
	_BV( 7 )	, // PK 7 ** 69 ** A15
};

static volatile uint8_t *
ddr_reg(uint8_t pin)
{
	return pgm_read_ptr(&port_to_mode_PGM[pgm_read_byte(&digital_pin_to_port_PGM[pin])]);
}

static volatile uint8_t *
pin_reg(uint8_t pin)
{
	return pgm_read_ptr(&port_to_input_PGM[pgm_read_byte(&digital_pin_to_port_PGM[pin])]);
}

static volatile uint8_t *
port_reg(uint8_t pin)
{
	return pgm_read_ptr(&port_to_output_PGM[pgm_read_byte(&digital_pin_to_port_PGM[pin])]);
}

static uint8_t
bit(uint8_t pin)
{
	return pgm_read_byte(&digital_pin_to_bit_mask_PGM[pin]);
}

static volatile uint8_t * const PROGMEM ocr_reg_addrs[NUM_PIN] = {
	[2] = &OCR3BL,
	[3] = &OCR3CL,
	[4] = &OCR0B,
	[5] = &OCR3AL,
	[6] = &OCR4AL,
	[7] = &OCR4BL,
	[8] = &OCR4CL,
	[9] = &OCR2B,
	[10] = &OCR2A,
	[11] = &OCR1AL,
	[12] = &OCR1BL,
	[13] = &OCR0A,
	[44] = &OCR5CL,
	[45] = &OCR5BL,
	[46] = &OCR5AL,
};

static volatile uint8_t *
ocr_reg(uint8_t pin) {
	return (volatile uint8_t *) pgm_read_ptr(&ocr_reg_addrs[pin]);
}

static bool
has_pwm(uint8_t p)
{
	return ocr_reg(p) != NULL;
}

static volatile uint8_t * const PROGMEM tcc_reg_addrs[] = {
	[2] = &TCCR3A,
	[3] = &TCCR3A,
	[4] = &TCCR0A,
	[5] = &TCCR3A,
	[6] = &TCCR4A,
	[7] = &TCCR4A,
	[8] = &TCCR4A,
	[9] = &TCCR2A,
	[10] = &TCCR2A,
	[11] = &TCCR1A,
	[12] = &TCCR1A,
	[13] = &TCCR0A,
	[44] = &TCCR5A,
	[45] = &TCCR5A,
	[46] = &TCCR5A,
};

static volatile uint8_t *
tcc_reg(uint8_t pin) {
	return (volatile uint8_t *) pgm_read_ptr(&tcc_reg_addrs[pin]);
}

static uint8_t const PROGMEM tcc_vals[] = {
	[2] = (0 << COM3B0) | (1 << COM3B1),
	[3] = (0 << COM3C0) | (1 << COM3C1),
	[4] = (0 << COM0B0) | (1 << COM0B1),
	[5] = (0 << COM3A0) | (1 << COM3A1),
	[6] = (0 << COM4A0) | (1 << COM4A1),
	[7] = (0 << COM4B0) | (1 << COM4B1),
	[8] = (0 << COM4C0) | (1 << COM4C1),
	[9] = (0 << COM2B0) | (1 << COM2B1),
	[10] = (0 << COM2A0) | (1 << COM2A1),
	[11] = (0 << COM1A0) | (1 << COM1A1),
	[12] = (0 << COM1B0) | (1 << COM1B1),
	[13] = (0 << COM0A0) | (1 << COM0A1),
	[44] = (0 << COM5C0) | (1 << COM5C1),
	[45] = (0 << COM5B0) | (1 << COM5B1),
	[46] = (0 << COM5A0) | (1 << COM5A1),
};

static uint8_t
tcc_val(uint8_t pin)
{
	return (uint8_t) pgm_read_byte(&tcc_vals[pin]);
}

static bool
is_pull(uint8_t pin)
{
	return (pull_pins[pin_byte(pin)] >> pin_bit(pin)) & 1;
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
	return (on_pins[pin_byte(pin)] >> pin_bit(pin)) & 1;
}

static void
set_on(uint8_t pin)
{
	on_pins[pin_byte(pin)] |= 1 << pin_bit(pin);
}

static void
set_off(uint8_t pin)
{
	on_pins[pin_byte(pin)] &= ~(1 << pin_bit(pin));
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
		pull_pins[pin_byte(p)] &= ~(1 << pin_bit(p));
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_pullup(snek_poly_t a)
{
	uint8_t p = snek_poly_get_pin(a);
	if (!snek_abort)
		pull_pins[pin_byte(p)] |= (1 << pin_bit(p));
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

	if (p >= A0) {
		uint8_t pin = p - A0;
		ADMUX = (analog_reference << REFS0) | (pin & 15);
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
		if (is_on(p)) {
			set_off(p);
			set_out(p);
		}
	return SNEK_NULL;
}

static uint32_t
snek_millis(void)
{
	uint32_t	millis;

	cli();
	millis = timer0_millis;
	sei();
	return millis;
}

snek_poly_t
snek_builtin_time_sleep(snek_poly_t a)
{
	uint32_t	expire = snek_millis() + (snek_poly_get_float(a) * 1000.0f + 0.5f);
	while (!snek_abort && (int32_t) (expire - snek_millis()) > 0)
	       ;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_time_monotonic(void)
{
	return snek_float_to_poly(snek_millis() / 1000.0f);
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
	return snek_float_to_poly((snek_soffset_t) (random_next % snek_poly_get_soffset(a)));
}
