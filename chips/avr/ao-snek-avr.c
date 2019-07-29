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

#include "ao.h"
#include "snek.h"
#include <snek-io.h>

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

#define PB	0
#define PC	1
#define PD	2
#define PE	3
#define PF	4

#define MAKE_MAP(P,B)	(((P) * 3) << 3 | (B))
#define MAP_P(M)	((M) >> 3)
#define MAP_B(M)	((M) & 7)

/*
 * GPIOs:
 *
 * 0	D0	PD2
 * 1	D1	PD3
 * 2	D2	PD1			SDA
 * 3	D3	PD0	PWM	OC0B	SCL
 * 4	D4	PD4
 * 5	D5	PC6	PWM	OC3A
 * 6	D6	PD7	PWM	OC4D
 * 7	D7	PE6
 * 8	D8	PB4
 * 9	D9	PB5	PWM	OC1A
 * 10	D10	PB6	PWM	OC1B
 * 11	D11	PB7	PWM	OC0A
 * 12	D12	PD6
 * 13	D13	PC7	PWM	OC4A
 * 14	A0	PF7	ADC	ADC7
 * 15	A1	PF6	ADC	ADC6
 * 16	A2	PF5	ADC	ADC5
 * 17	A3	PF4	ADC	ADC4
 * 18	A4	PF1	ADC	ADC1
 * 19	A5	PF0	ADC	ADC0
 * 20	MISO	PB3			MISO
 * 21	MOSI	PB2			MOSI
 * 22	SCK	PB1			SCK
 */

#define NUM_PIN		23
#define NUM_ADC		6
#define FIRST_ADC	14

static uint8_t	power[NUM_PIN];
static uint8_t	power_pin;
static uint8_t	dir_pin;
static uint8_t	on_pins[NUM_PIN];

#define SNEK_PULL_NONE	0
#define SNEK_PULL_UP	1

/* digital pins all use PULL_UP by default */
static uint8_t	pull[NUM_PIN] = {
	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1, 1, 1,
	1, 1
};

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

	/* Timer 1 (clkio/64) */
	TCCR1B = ((0 << CS12) |
		  (1 << CS11) |
		  (1 << CS10));

	/* Phase correct 8-bit */
	TCCR1A = ((0 << WGM12) |
		  (0 << WGM11) |
		  (1 << WGM10));

	OCR1AH = 0;
	OCR1BH = 0;

	/* Timer 3 (clkio/64) */
	TCCR3B = ((0 << CS32) |
		  (1 << CS31) |
		  (1 << CS30));

	TCCR3A = ((1 << WGM30));

	/* Timer 4 */

	/* PWM mode */
	TCCR4A = ((1 << PWM4A) |
		  (1 << PWM4B));

	/* (clkio/64) */
	TCCR4B = ((0 << CS43) |
		  (1 << CS42) |
		  (1 << CS41) |
		  (1 << CS40));

	/* Phase correct */
	TCCR4D = ((0 << WGM41) |
		  (1 << WGM40));

	/* count to 255 */
	TC4H = 0;
	OCR4C = 0xff;

	memset(power, 0xff, NUM_PIN);
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
	DDRE = 0;
	DDRF = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	PORTE = 0;
	PORTF = 0;
}

#include <avr/wdt.h>
#include <setjmp.h>

static jmp_buf	snek_reset_buf;

int __attribute__((OS_main))
main (void)
{
	stderr = stdout = stdin = &snek_avr_file;
	ao_usb_init();

	setjmp(snek_reset_buf);
	port_init();
	snek_init();
	fprintf(stdout, "Welcome to snek " SNEK_VERSION "\n");
	fflush(stdout);
	for (;;)
		snek_parse();
}

static const PROGMEM uint8_t _pin_map[NUM_PIN] = PIN_MAP;
static const PROGMEM uint8_t _adc_map[NUM_ADC] = ADC_MAP;
static uint8_t const PROGMEM ocr_reg_addrs[NUM_PIN] = OCR_REG_ADDRS;
static volatile uint8_t const PROGMEM tcc_reg_addrs[] = TCC_REG_ADDRS;
static uint8_t const PROGMEM tcc_reg_vals[] = TCC_REG_VALS;

static uint8_t
pin_map(uint8_t pin)
{
	return pgm_read_byte(&_pin_map[pin]);
}

static volatile uint8_t *
ddr_reg(uint8_t pin)
{
	return (volatile uint8_t *) (uintptr_t) (0x24 + MAP_P(pin_map(pin)));
}

static volatile uint8_t *
pin_reg(uint8_t pin)
{
	return (volatile uint8_t *) (uintptr_t) (0x23 + MAP_P(pin_map(pin)));
}

static volatile uint8_t *
port_reg(uint8_t pin)
{
	return (volatile uint8_t *) (uintptr_t) (0x25 + MAP_P(pin_map(pin)));
}

static uint8_t
bit(uint8_t pin)
{
	return 1 << MAP_B(pin_map(pin));
}

static bool
has_adc(uint8_t p)
{
	return FIRST_ADC <= p && p < (FIRST_ADC + NUM_ADC);
}

static volatile uint8_t *
ocr_reg(uint8_t pin) {
	return (volatile uint8_t *) (uintptr_t) pgm_read_byte(&ocr_reg_addrs[pin]);
}

static bool
has_pwm(uint8_t p)
{
	return ocr_reg(p) != NULL;
}

static volatile uint8_t *
tcc_reg(uint8_t pin) {
	return (volatile uint8_t *) (uintptr_t) pgm_read_byte(&tcc_reg_addrs[pin]);
}


static uint8_t
tcc_reg_val(uint8_t pin)
{
	return (uint8_t) pgm_read_byte(&tcc_reg_vals[pin]);
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
		if (pull[pin])
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

static inline bool
is_on(uint8_t pin)
{
	return on_pins[pin];
}

static inline void
set_on(uint8_t pin)
{
	on_pins[pin] = 1;
}

static inline void
set_off(uint8_t pin)
{
	on_pins[pin] = 0;
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
			*tcc_reg(pin) |= tcc_reg_val(pin);
			return SNEK_NULL;
		}
		*tcc_reg(pin) &= ~tcc_reg_val(pin);
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
		pull[p] = SNEK_PULL_NONE;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_pullup(snek_poly_t a)
{
	uint8_t p = snek_poly_get_pin(a);
	if (!snek_abort)
		pull[p] = SNEK_PULL_UP;
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

	if (has_adc(p) && !pull[p]) {
		uint8_t pin = pgm_read_byte(&_adc_map[p - FIRST_ADC]);
		ADMUX = (analog_reference << REFS0) | (pin & 7);
		ADCSRA |= (1 << ADSC);
		while (ADCSRA & (1 << ADSC))
			;
		uint8_t low = ADCL;
		uint8_t high = ADCH;
		float value = ((uint16_t) high << 8 | low) / 1023.0f;

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
	ao_arch_nop();
	sei();
	ao_arch_nop();
	return millis;
}

snek_poly_t
snek_builtin_time_sleep(snek_poly_t a)
{
	uint32_t	expire = snek_millis() + (snek_poly_get_float(a) * 1000.0f + 0.5f);
	while (!snek_abort && (int32_t) (expire - snek_millis()) > 0)
		ao_arch_nop();
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_time_monotonic(void)
{
	return snek_float_to_poly(snek_millis() / 1000.0f);
}

static uint16_t random_next;

snek_poly_t
snek_builtin_random_seed(snek_poly_t a)
{
	random_next = a.u;
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_random_randrange(snek_poly_t a)
{
	random_next = random_next * 31421 + 6927;
	return snek_float_to_poly(random_next % snek_poly_get_soffset(a));
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
	memcpy_P(&__snek_data_start__,
		 (&__text_end__ + (&__snek_data_start__ - &__data_start__)),
		  &__snek_data_end__ - &__snek_data_start__);

	/* reset bss */
	memset(&__snek_bss_start__, '\0', &__snek_bss_end__ - &__snek_bss_start__);

	/* and off we go! */
	longjmp(snek_reset_buf, 1);
	return SNEK_NULL;
}
