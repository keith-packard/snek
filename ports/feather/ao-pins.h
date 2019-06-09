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

/* Run dffl48m clock at 48MHz */
#define AO_DFLL48M		48000000

/* Reference clock is 32.768kHz */
#define AO_XOSC32K		32768

#define AO_AHB_PRESCALER	1
#define AO_APBA_PRESCALER	1

#define HAS_SERIAL_0		1
#define USE_SERIAL_0_STDIN	0
#define SERIAL_0_PA10_PA11	1

#define HAS_USB			1
#define AO_USB_OUT_HOOK		1
#define USE_USB_FIFO		1
#define HAS_BEEP		0

#define AO_SNEK_TIMER_FUNC_E	0
#define AO_SNEK_TIMER_FUNC_F	0x80
#define AO_SNEK_TIMER_FUNC_MASK	0x80
#define AO_SNEK_TIMER_DEV_TCC	0
#define AO_SNEK_TIMER_DEV_TC	0x40
#define AO_SNEK_TIMER_DEV_MASK	0x40
#define AO_SNEK_TIMER_CH_MASK	0x0f

#define AO_SNEK_TIMER_CH(ch,func,dev)	((ch) | (func) | (dev))

/*
 *
 *	Name	Pin	E	F		tcc0	 tcc1  tcc2  tc3  tc4  tc5
 *						0 1 2 3  0 1   0 1   0 1  0 1  0 1
 *                                              * * * *  * *   * *   * *
 *	D2	PA14	TC3_0	TCC0_4                                                  -
 *	D3	PA9	TCC0_1	TCC1_3            1                                     TCC0_1
 *	D4	PA8	TCC0_0	TCC1_2          0                                       TCC0_0
 *	D5	PA15	TC3_1	TCC0_5                                 1                TC3_1
 *	D6	PA20		TCC0_6              6                                   TCC0_6
 *	D7	PA21		TCC0_7                7                                 TCC0_7
 *	D8	PA6	TCC1_0                           0                              TCC1_0
 *	D9	PA7	TCC1_1                             1                            TCC1_1
 * 	D10	PA18	TC3_0	TCC0_2                              0                   TC3_0
 *	D11	PA16	TCC2_0	TCC0_6                         0			TCC2_0
 *	D12	PA19	TC3_1	TCC0_3                                                  -
 *	D13	PA17	TCC2_1	TCC0_7                           1                      TCC2_1
 * tcc0
 *	0	D4	E
 *	1	D3	E
 *	2
 *	3
 *	4
 *	5
 *	6	D6	F
 *	7	D7	F
 * tcc1
 *	0	D8	E
 *	1	D9	E
 * tcc2
 *	0	D11	E
 *	1	D13	E
 * tcc3
 *	doesn't appear to exist in this chip
 * tc3
 *	0	D10	E	D5	E
 *	1	D12	E	D2	E
 * tc4
 *	0
 *	1
 * tc5
 *	0
 *	1
 * tc6
 *	0
 *	1
 * tc7
 *	0	pin 6	E
 *	1	pin 7	E
 *
 */

#define AO_SNEK_ADC_0	0xff
#define AO_SNEK_ADC_1	0xff
#define AO_SNEK_ADC_2	0xff
#define AO_SNEK_ADC_3	0xff
#define AO_SNEK_ADC_4	0xff
#define AO_SNEK_ADC_5	0xff
#define AO_SNEK_ADC_6	0xff
#define AO_SNEK_ADC_7	0xff
#define AO_SNEK_ADC_8	0xff
#define AO_SNEK_ADC_9	0xff
#define AO_SNEK_ADC_10	0xff
#define AO_SNEK_ADC_11	0xff
#define AO_SNEK_ADC_12	0xff
#define AO_SNEK_ADC_13	0xff

#define AO_SNEK_GPIO_0		(&samd21_port_a)
#define AO_SNEK_PIN_0		11
#define AO_SNEK_TIMER_0		NULL
#define AO_SNEK_CHANNEL_0	0

#define AO_SNEK_GPIO_1		(&samd21_port_a)
#define AO_SNEK_PIN_1		10
#define AO_SNEK_TIMER_1		NULL
#define AO_SNEK_CHANNEL_1	0

#define AO_SNEK_GPIO_2		(&samd21_port_a)		/* arduino D4 */
#define AO_SNEK_PIN_2		14
#define AO_SNEK_TIMER_2		NULL
#define AO_SNEK_CHANNEL_2	0

#define AO_SNEK_GPIO_3		(&samd21_port_a)		/* arduino D3 */
#define AO_SNEK_PIN_3		9
#define AO_SNEK_TIMER_3		(&samd21_tcc0)
#define AO_SNEK_CHANNEL_3	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)

#define AO_SNEK_GPIO_4		(&samd21_port_a)		/* arduino D2 */
#define AO_SNEK_PIN_4		8
#define AO_SNEK_TIMER_4		(&samd21_tcc0)
#define AO_SNEK_CHANNEL_4	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)

#define AO_SNEK_GPIO_5		(&samd21_port_a)	       /* arduino D5 */
#define AO_SNEK_PIN_5		15
#define AO_SNEK_TIMER_5		(&samd21_tc3)
#define AO_SNEK_CHANNEL_5	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)

#define AO_SNEK_GPIO_6		(&samd21_port_a)		/* arduino D6 */
#define AO_SNEK_PIN_6		20
#define AO_SNEK_TIMER_6		(&samd21_tcc0)
#define AO_SNEK_CHANNEL_6	AO_SNEK_TIMER_CH(6 & 3, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)

#define AO_SNEK_GPIO_7		(&samd21_port_a)		/* arduino D7 */
#define AO_SNEK_PIN_7		21
#define AO_SNEK_TIMER_7		(&samd21_tcc0)
#define AO_SNEK_CHANNEL_7	AO_SNEK_TIMER_CH(7 & 3, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)

#define AO_SNEK_GPIO_8		(&samd21_port_a)		/* arduino D8 */
#define AO_SNEK_PIN_8		6
#define AO_SNEK_TIMER_8		(&samd21_tcc1)
#define AO_SNEK_CHANNEL_8	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)

#define AO_SNEK_GPIO_9		(&samd21_port_a)		/* arduino D9 */
#define AO_SNEK_PIN_9		7
#define AO_SNEK_TIMER_9		(&samd21_tcc1)
#define AO_SNEK_CHANNEL_9	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)

#define AO_SNEK_GPIO_10		(&samd21_port_a)		/* arduino D10 */
#define AO_SNEK_PIN_10		18
#define AO_SNEK_TIMER_10	(&samd21_tc3)
#define AO_SNEK_CHANNEL_10	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)

#define AO_SNEK_GPIO_11		(&samd21_port_a)		/* ardunio D11 */
#define AO_SNEK_PIN_11		16
#define AO_SNEK_TIMER_11	(&samd21_tcc2)
#define AO_SNEK_CHANNEL_11	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)

#define AO_SNEK_GPIO_12		(&samd21_port_a)		/* arduino D12 */
#define AO_SNEK_PIN_12		19
#define AO_SNEK_TIMER_12	NULL
#define AO_SNEK_CHANNEL_12	0

#define AO_SNEK_GPIO_13		(&samd21_port_a)		/* arduino D13 */
#define AO_SNEK_PIN_13		17
#define AO_SNEK_TIMER_13	(&samd21_tcc2)
#define AO_SNEK_CHANNEL_13	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)

/* analog */
#define AO_SNEK_GPIO_14		(&samd21_port_a)
#define AO_SNEK_PIN_14		2
#define AO_SNEK_TIMER_14	NULL
#define AO_SNEK_CHANNEL_14	0
#define AO_SNEK_ADC_14		0

#define AO_SNEK_GPIO_15		(&samd21_port_b)
#define AO_SNEK_PIN_15		8
#define AO_SNEK_TIMER_15	NULL
#define AO_SNEK_CHANNEL_15	0
#define AO_SNEK_ADC_15		2

#define AO_SNEK_GPIO_16		(&samd21_port_b)
#define AO_SNEK_PIN_16		9
#define AO_SNEK_TIMER_16	NULL
#define AO_SNEK_CHANNEL_16	0
#define AO_SNEK_ADC_16		3

#define AO_SNEK_GPIO_17		(&samd21_port_a)
#define AO_SNEK_PIN_17		4
#define AO_SNEK_TIMER_17	NULL
#define AO_SNEK_CHANNEL_17	0
#define AO_SNEK_ADC_17		4

#define AO_SNEK_GPIO_18		(&samd21_port_a)
#define AO_SNEK_PIN_18		5
#define AO_SNEK_TIMER_18	NULL
#define AO_SNEK_CHANNEL_18	0
#define AO_SNEK_ADC_18		5

#define AO_SNEK_GPIO_19		(&samd21_port_b)
#define AO_SNEK_PIN_19		2
#define AO_SNEK_TIMER_19	NULL
#define AO_SNEK_CHANNEL_19	0
#define AO_SNEK_ADC_19		10

/* SDA */
#define AO_SNEK_GPIO_20		(&samd21_port_a)
#define AO_SNEK_PIN_20		22
#define AO_SNEK_TIMER_20	NULL
#define AO_SNEK_CHANNEL_20	0
#define AO_SNEK_ADC_20		0xff

/* SCL */
#define AO_SNEK_GPIO_21		(&samd21_port_a)
#define AO_SNEK_PIN_21		23
#define AO_SNEK_TIMER_21	NULL
#define AO_SNEK_CHANNEL_21	0
#define AO_SNEK_ADC_21		0xff

/* SCK */
#define AO_SNEK_GPIO_22		(&samd21_port_b)
#define AO_SNEK_PIN_22		11
#define AO_SNEK_TIMER_22	NULL
#define AO_SNEK_CHANNEL_22	0
#define AO_SNEK_ADC_22		0xff

/* MOSI */
#define AO_SNEK_GPIO_23		(&samd21_port_b)
#define AO_SNEK_PIN_23		10
#define AO_SNEK_TIMER_23	NULL
#define AO_SNEK_CHANNEL_23	0
#define AO_SNEK_ADC_23		0xff

/* MISO */
#define AO_SNEK_GPIO_24		(&samd21_port_a)
#define AO_SNEK_PIN_24		12
#define AO_SNEK_TIMER_24	NULL
#define AO_SNEK_CHANNEL_24	0
#define AO_SNEK_ADC_24		0xff

#define AO_SNEK_NUM_PIN		25

#define LED_0_PORT		(&samd21_port_a)
#define LED_0_PIN		27
#define LED_PIN_TX		0
#define AO_LED_TX		(1 << LED_PIN_TX)
#define AO_LED_PANIC		AO_LED_TX

#define LEDS_AVAILABLE		(AO_LED_TX)

#endif /* _AO_PINS_H_ */
