/*
 * Copyright © 2012 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#ifndef _AO_PINS_H_
#define _AO_PINS_H_

/* Run fdpll96m clock from a 16MHz clock divided to 62.5kHz, multiplied to 48MHz
 * Attempts to use a PLL input higher than 250kHz failed, so we'll use something
 * as low as possible, which means above 32kHz
 */
#define AO_XOSC			1
#define AO_XOSC_FREQ		16000000
#define AO_XOSC_DIV		256
#define AO_XOSC_MUL		768

#define AO_AHB_PRESCALER	1
#define AO_APBA_PRESCALER	1

#define HAS_SERIAL_0		0
#define USE_SERIAL_0_STDIN	0
#define SERIAL_0_PA10_PA11	0

#define HAS_USB			1
#define AO_USB_OUT_HOOK		1
#define USE_USB_FIFO		1

#define AO_SNEK_TIMER_FUNC_E	0
#define AO_SNEK_TIMER_FUNC_F	0x80
#define AO_SNEK_TIMER_FUNC_MASK	0x80
#define AO_SNEK_TIMER_DEV_TCC	0
#define AO_SNEK_TIMER_DEV_TC	0x40
#define AO_SNEK_TIMER_DEV_MASK	0x40
#define AO_SNEK_TIMER_CH_MASK	0x0f

#define AO_SNEK_TIMER_CH(ch,func,dev)	((ch) | (func) | (dev))

/*
 * Available timers:
 *
 *	TCC0.[0-7]	0-3 overlap 4-7		0,1,2,3
 *	TCC1.[0-3]	0-1 overlap 2-3		0,1
 *	TCC2.[0-1]				0,1
 *	TC3.[0-1]				0,1
 *	TC4.[0-1]				0,1
 *	TC5.[0-1]
 *
 *	1	PA00
 *	2	PA01
 *	3	PA02	AIN0
 *	4	PA03	AIN1
 *	9	PA04	AIN4	TCC0.0/E
 *	10	PA05	AIN5	TCC0.1/E
 *	11	PA06	AIN6	TCC1.0/E		ANALOG3	TCC1.0/E
 *	12	PA07	AIN7	TCC1.1/E		ANALOG4 TCC1.0/E
 * 	13	PA08	AIN16	TCC0.0/E TCC1.2/F	ANALOG5 TCC0.0/E
 * 	14	PA09	AIN17	TCC0.1/E TCC1.3/F	ANALOG6	TCC0.1/E
 * 	15	PA10	AIN18	TCC1.0/E TCC0.2/F	ANALOG7 TCC0.2/F
 * 	16	PA11	AIN19	TCC1.1/E TCC0.3/F	ANALOG8 TCC0.3/F
 *	21	PA12		TCC2.0/E TCC0.6/F	M1_PWR	TCC2.0/E
 *	22	PA13		TCC2.1/E TCC0.7/F	M2_PWR	TCC2.1/E
 *	23	PA14					XOSC
 *	24	PA15					XOSC
 *	25	PA16		TCC2.0/E TCC0.6/F	M1_DIR
 *	26	PA17		TCC2.1/E TCC0.7/F	M2_DIR
 *	27	PA18		TC3.0/E TCC0.2/F	M3_PWR TC3.0/E
 *	28	PA19		TC3.1/E TCC0.3/F	M4_PWR TC3.1/E
 *	29	PA20					M3_DIR
 *	30	PA21					M4_DIR
 *	31	PA22					DIGITAL1
 *	32	PA23					DIGITAL2
 *	33	PA24		TC5.0			DATA-
 *	34	PA25		TC5.1			DATA+
 *	39	PA27					DIGITAL3
 *	41	PA28					DIGITAL4
 *	45	PA30					DIGITAL5
 *	46	PA31					DIGITAL6
 *	none		AIN8
 *	none		AIN9
 * 	47	PB02	AIN10				DIGITAL7
 * 	48	PB03	AIN11				DIGITAL8
 *	7	PB08	AIN2	TC4.0/E			ANALOG1 TC4.0/E
 *	8	PB09	AIN3	TC4.1/E			ANALOG2 TC4.1/E
 *	19	PB10		TC5.0
 *	20	PB11		TC5.1
 *	37	PB22
 *	38	PB23

 * ANALOG1	PB08	AIN2	TC4.0/E		0
 * ANALOG2	PB09	AIN3	TC4.1/E		1
 * ANALOG3	PA06	AIN6	TCC1.0/E	2
 * ANALOG4	PA07	AIN7	TCC1.1/E	3
 * ANALOG5	PA08	AIN16	TCC0.0/E	4
 * ANALOG6	PA09	AIN17	TCC0.1/E	5
 * ANALOG7	PA10	AIN18	TCC0.2/F	6
 * ANALOG8	PA11	AIN19	TCC0.3/F	7
 *
 * M1_PWR	PA12		TCC2.0/E	8
 * M1_DIR	PA16				9
 * M2_PWR	PA13		TCC2.1/E	10
 * M2_DIR	PA17				11
 * M3_PWR	PA18		TC3.0/E		12
 * M3_DIR	PA20				13
 * M4_PWR	PA19		TC3.1/E		14
 * M4_DIR	PA21				15
 *
 * NEOPIXEL	PB11				16
 *
 * LED		PA02
 */

/* ANALOG1 */
#define AO_SNEK_GPIO_0		(&samd21_port_b)
#define AO_SNEK_PIN_0		8
#define AO_SNEK_TIMER_0		(&samd21_tc4)
#define AO_SNEK_CHANNEL_0	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_0		2

/* ANALOG2 */
#define AO_SNEK_GPIO_1		(&samd21_port_b)
#define AO_SNEK_PIN_1		9
#define AO_SNEK_TIMER_1		(&samd21_tc4)
#define AO_SNEK_CHANNEL_1	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_1		3

/* ANALOG3 */
#define AO_SNEK_GPIO_2		(&samd21_port_a)
#define AO_SNEK_PIN_2		6
#define AO_SNEK_TIMER_2		(&samd21_tcc1)
#define AO_SNEK_CHANNEL_2	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_2		6

/* ANALOG4 */
#define AO_SNEK_GPIO_3		(&samd21_port_a)
#define AO_SNEK_PIN_3		7
#define AO_SNEK_TIMER_3		(&samd21_tcc1)
#define AO_SNEK_CHANNEL_3	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_3		7

/* ANALOG5 */
#define AO_SNEK_GPIO_4		(&samd21_port_a)
#define AO_SNEK_PIN_4		8
#define AO_SNEK_TIMER_4		(&samd21_tcc0)
#define AO_SNEK_CHANNEL_4	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_4		16

/* ANALOG6 */
#define AO_SNEK_GPIO_5		(&samd21_port_a)
#define AO_SNEK_PIN_5		9
#define AO_SNEK_TIMER_5		(&samd21_tcc0)
#define AO_SNEK_CHANNEL_5	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_5		17

/* ANALOG7 */
#define AO_SNEK_GPIO_6		(&samd21_port_a)
#define AO_SNEK_PIN_6		10
#define AO_SNEK_TIMER_6		(&samd21_tcc0)
#define AO_SNEK_CHANNEL_6	AO_SNEK_TIMER_CH(2, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_6		18

/* ANALOG8 */
#define AO_SNEK_GPIO_7		(&samd21_port_a)
#define AO_SNEK_PIN_7		11
#define AO_SNEK_TIMER_7		(&samd21_tcc0)
#define AO_SNEK_CHANNEL_7	AO_SNEK_TIMER_CH(3, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_7		19

/* MOTOR1_POWER */
#define AO_SNEK_GPIO_8		(&samd21_port_a)
#define AO_SNEK_PIN_8		12
#define AO_SNEK_TIMER_8		(&samd21_tcc2)
#define AO_SNEK_CHANNEL_8	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_8		0xff
#define AO_SNEK_FLAGS_8		SNEK_PIN_RAMP_PWM

/* MOTOR1_DIR */
#define AO_SNEK_GPIO_9		(&samd21_port_a)
#define AO_SNEK_PIN_9		16
#define AO_SNEK_TIMER_9		NULL
#define AO_SNEK_CHANNEL_9	0
#define AO_SNEK_ADC_9		0xff

/* MOTOR2_POWER */
#define AO_SNEK_GPIO_10		(&samd21_port_a)
#define AO_SNEK_PIN_10		13
#define AO_SNEK_TIMER_10	(&samd21_tcc2)
#define AO_SNEK_CHANNEL_10	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_10		0xff
#define AO_SNEK_FLAGS_10	SNEK_PIN_RAMP_PWM

/* MOTOR2_DIR */
#define AO_SNEK_GPIO_11		(&samd21_port_a)
#define AO_SNEK_PIN_11		17
#define AO_SNEK_TIMER_11	NULL
#define AO_SNEK_CHANNEL_11	0
#define AO_SNEK_ADC_11		0xff

/* MOTOR3_POWER */
#define AO_SNEK_GPIO_12		(&samd21_port_a)
#define AO_SNEK_PIN_12		18
#define AO_SNEK_TIMER_12	(&samd21_tc3)
#define AO_SNEK_CHANNEL_12	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_12		0xff
#define AO_SNEK_FLAGS_12	SNEK_PIN_RAMP_PWM

/* MOTOR3_DIR */
#define AO_SNEK_GPIO_13		(&samd21_port_a)
#define AO_SNEK_PIN_13		20
#define AO_SNEK_TIMER_13	NULL
#define AO_SNEK_CHANNEL_13	0
#define AO_SNEK_ADC_13		0xff

/* MOTOR4_POWER */
#define AO_SNEK_GPIO_14		(&samd21_port_a)
#define AO_SNEK_PIN_14		19
#define AO_SNEK_TIMER_14	(&samd21_tc3)
#define AO_SNEK_CHANNEL_14	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_14		0xff
#define AO_SNEK_FLAGS_14	SNEK_PIN_RAMP_PWM

/* MOTOR4_DIR */
#define AO_SNEK_GPIO_15		(&samd21_port_a)
#define AO_SNEK_PIN_15		21
#define AO_SNEK_TIMER_15	NULL
#define AO_SNEK_CHANNEL_15	0
#define AO_SNEK_ADC_15		0xff

/* NEOPIXEL */
#define AO_SNEK_GPIO_16		(&samd21_port_b)
#define AO_SNEK_PIN_16  	11
#define AO_SNEK_TIMER_16	NULL
#define AO_SNEK_CHANNEL_16	0
#define AO_SNEK_ADC_16		0xff

#define AO_SNEK_NUM_PIN		17

#define LED_0_PORT		(&samd21_port_a)
#define LED_0_PIN		2
#define LED_PIN_TX		0
#define AO_LED_TX		(1 << LED_PIN_TX)
#define AO_LED_PANIC		AO_LED_TX

#define LEDS_AVAILABLE		(AO_LED_TX)

#endif /* _AO_PINS_H_ */
