/*
 * Copyright © 2021 Keith Packard <keithp@keithp.com>
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
#include "snek-io.h"

#define NUM_PIN	22
#define A0	14

static uint8_t	power_pin;
static uint8_t	dir_pin;
static uint16_t	power[NUM_PIN];
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

static void
port_init(void)
{
	uint8_t p;

	/* Enable ADC */
	ADC0_CTRLC = (ADC_SAMPCAP_bm | ADC_REFSEL0_bm | ADC_PRESC_DIV16_gc);
	ADC0_CTRLA = ADC_ENABLE_bm;

	/* TCA0 */
	TCA0_SINGLE_CTRLB = (TCA_SINGLE_WGMODE_SINGLESLOPE_gc);
	TCA0_SINGLE_CTRLA = (TCA_SINGLE_ENABLE_bm);

	/* Enable interrupt */
	TCA0_SINGLE_INTCTRL = (TCA_SINGLE_OVF_bm);

	/* TCA0 drives PB0-2 */
	PORTMUX_TCAROUTEA = PORTMUX_TCA0_PORTB_gc;

	/* TCB0 */
	TCB0_CTRLB = (TCB_CNTMODE_PWM8_gc);
	TCB0_CTRLA = (TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm);

	/* TCB1 */
	TCB1_CTRLB = (TCB_CNTMODE_PWM8_gc);
	TCB1_CTRLA = (TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm);

	/* TCB0 drives PF4, TCB1 drives PF5 */
	PORTMUX_TCBROUTEA = PORTMUX_TCB0_bm | PORTMUX_TCB1_bm;

	memset(power, 0xff, sizeof(power));
	memset(pull_pins, 0x00, sizeof(pull_pins));
	for (p = 0; p < A0; p++)
		pull_pins[pin_byte(p)] |= (1 << pin_bit(p));
}

FILE snek_duino_file = FDEV_SETUP_STREAM(snek_uart_putchar, snek_eeprom_getchar, _FDEV_SETUP_RW);

int __attribute__((OS_main))
main (void)
{
	_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0);
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
	_PROTECTED_WRITE(RSTCTRL_SWRR, RSTCTRL_SWRE_bm);
	return SNEK_NULL;
}

#define PA 1
#define PB 2
#define PC 3
#define PD 4
#define PE 5
#define PF 6

#define NOT_A_PIN 0
#define NOT_A_PORT 0

typedef volatile uint8_t vuint8_t;

static VPORT_t * PROGMEM const port_to_vport_PGM[] = {
	NOT_A_PORT,
	&VPORTA,
	&VPORTB,
	&VPORTC,
	&VPORTD,
	&VPORTE,
	&VPORTF,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	// PORTLIST
	// -------------------------------------------
	PB	, // PB 4 ** 0 ** USART0_RX
	PB	, // PB 5 ** 1 ** USART0_TX
	PA	, // PA 0 ** 2 ** PWM2
	PF	, // PF 5 ** 3 ** PWM3
	PC	, // PC 6 ** 4 ** PWM4
	PB	, // PB 2 ** 5 ** PWM5
	PF	, // PF 4 ** 6 ** PWM6
	PA	, // PA 1 ** 7 ** PWM7
	PE	, // PE 3 ** 8 ** PWM8
	PB	, // PB 0 ** 9 ** PWM9
	PB	, // PB 1 ** 10 ** PWM10
	PE	, // PE 0 ** 11 ** PWM11 MOSI
	PE	, // PE 1 ** 12 ** PWM12 MISO
	PE	, // PE 2 ** 13 ** SCK
	PD	, // PD 3 ** 14 ** A0
	PD	, // PD 2 ** 15 ** A1
	PD	, // PD 1 ** 16 ** A2
	PD	, // PD 0 ** 17 ** A3
	PA	, // PA 2 ** 18 ** A4
	PA	, // PA 3 ** 19 ** A5
	PD	, // PD 4 ** 20 ** A6
	PD	, // PD 5 ** 21 ** A7
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	// PIN IN PORT
	// -------------------------------------------
	_BV(4)	, // PB 4 ** 0 ** USART0_RX
	_BV(5)	, // PB 5 ** 1 ** USART0_TX
	_BV(0)	, // PA 0 ** 2
	_BV(5)	, // PF 5 ** 3 ** PWM
	_BV(6)	, // PC 6 ** 4
	_BV(2)	, // PB 2 ** 5 ** PWM5
	_BV(4)	, // PF 4 ** 6
	_BV(1)	, // PA 1 ** 7
	_BV(3)	, // PE 3 ** 8
	_BV(0)	, // PB 0 ** 9 ** PWM9
	_BV(1)	, // PB 1 ** 10 ** PWM10
	_BV(0)	, // PE 0 ** 11 ** MOSI
	_BV(1)	, // PE 1 ** 12 ** MISO
	_BV(2)	, // PE 2 ** 13 ** SCK
	_BV(3)	, // PD 3 ** 14 ** A0
	_BV(2)	, // PD 2 ** 15 ** A1
	_BV(1)	, // PD 1 ** 16 ** A2
	_BV(0)	, // PD 0 ** 17 ** A3
	_BV(2)	, // PA 2 ** 18 ** A4
	_BV(3)	, // PA 3 ** 19 ** A5
	_BV(4)	, // PD 4 ** 20 ** A6
	_BV(5)	, // PD 5 ** 21 ** A7
};

static VPORT_t *
vport(uint8_t pin)
{
	return pgm_read_ptr(&(port_to_vport_PGM[pgm_read_byte(&digital_pin_to_port_PGM[pin])]));
}

static volatile uint8_t *
ddr_reg(uint8_t pin)
{
	return &(vport(pin)->DIR);
}

static volatile uint8_t *
pin_reg(uint8_t pin)
{
	return &(vport(pin)->IN);
}

static volatile uint8_t *
port_reg(uint8_t pin)
{
	return &(vport(pin)->OUT);
}

static uint8_t
bit(uint8_t pin)
{
	return pgm_read_byte(&digital_pin_to_bit_mask_PGM[pin]);
}

static volatile uint16_t * const PROGMEM ccmp_reg_addrs[NUM_PIN] = {
	[3] = &TCB1_CCMP,
	[5] = &TCA0_SINGLE_CMP2,
	[6] = &TCB0_CCMP,
	[9] = &TCA0_SINGLE_CMP0,
	[10] = &TCA0_SINGLE_CMP1,
};

static volatile uint16_t *
ccmp_reg(uint8_t pin) {
	return (volatile uint16_t *) pgm_read_ptr(&ccmp_reg_addrs[pin]);
}

static bool
has_pwm(uint8_t p)
{
	return ccmp_reg(p) != NULL;
}

static volatile uint8_t * const PROGMEM tcc_reg_addrs[NUM_PIN] = {
	[3] = &TCB1_CTRLB,
	[5] = &TCA0_SINGLE_CTRLB,
	[6] = &TCB0_CTRLB,
	[9] = &TCA0_SINGLE_CTRLB,
	[10] = &TCA0_SINGLE_CTRLB,
};

static volatile uint8_t *
tcc_reg(uint8_t pin) {
	return (volatile uint8_t *) pgm_read_ptr(&tcc_reg_addrs[pin]);
}

static uint8_t const PROGMEM tcc_vals[NUM_PIN] = {
	[3] = TCB_CCMPEN_bm,
	[5] = TCA_SINGLE_CMP2EN_bm,
	[6] = TCB_CCMPEN_bm,
	[9] = TCA_SINGLE_CMP0EN_bm,
	[10] = TCA_SINGLE_CMP1EN_bm,
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
	uint16_t	p = 0;

	if (is_on(pin))
		p = power[pin];

	if (has_pwm(pin)) {
		if (0 < p && p < 65535) {
			/*
			 * TCB timers are only 8 bits, with the low 8
			 * bits being the period. We set that to 0xff
			 * to make them run as slow as possible. Even
			 * so, these PWM outputs run at 39kHz. Sigh.
			 */
			if (tcc_reg(pin) != &TCA0_SINGLE_CTRLB)
				p |= 0xff;
			*ccmp_reg(pin) = p;
			*tcc_reg(pin) |= tcc_val(pin);
			return SNEK_NULL;
		}
		*tcc_reg(pin) &= ~tcc_val(pin);
	}
	volatile uint8_t *out = port_reg(pin);
	if (p)
		*out |= bit(pin);
	else
		*out &= ~bit(pin);
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_setpower(snek_poly_t a)
{
	float p = snek_poly_get_float(a);
	if (p < 0.0f) p = 0.0f;
	if (p > 1.0f) p = 1.0f;
	power[power_pin] = (uint16_t) (p * 65535.0f + 0.5f);
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

static uint8_t const PROGMEM mux_pos_vals[] = {
	[0] = ADC_MUXPOS_AIN3_gc,
	[1] = ADC_MUXPOS_AIN2_gc,
	[2] = ADC_MUXPOS_AIN1_gc,
	[3] = ADC_MUXPOS_AIN0_gc,
	[4] = ADC_MUXPOS_AIN12_gc,
	[5] = ADC_MUXPOS_AIN13_gc,
	[6] = ADC_MUXPOS_AIN4_gc,
	[7] = ADC_MUXPOS_AIN5_gc,
};

static uint8_t
mux_pos(uint8_t a_pin) {
	return (uint8_t) pgm_read_byte(&mux_pos_vals[a_pin]);
}

snek_poly_t
snek_builtin_read(snek_poly_t a)
{
	uint8_t p = snek_poly_get_pin(a);
	if (snek_abort)
		return SNEK_NULL;
	set_dir(p, 0);

	if (p >= A0) {
		uint8_t pin = p - A0;

		ADC0_MUXPOS = mux_pos(pin);

		ADC0_COMMAND = ADC_STCONV_bm;
		while (ADC0_COMMAND & ADC_STCONV_bm)
			;

		uint16_t result = ADC0_RES;
		float value = result / 1023.0f;

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
