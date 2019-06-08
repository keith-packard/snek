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
 *	Available:
 *
 *	tcc0, tcc3	0-7 (4-7 overlay 0-3)
 *	tcc1		0-3 (2-3 overlay 0-1)
 *	tcc2		0-1
 *	TC6-7 not available
 *
 *	
 *	Names		Pin	E	F	Arduino		Snek
 *
 *	A0/SPEAKER	PA02		TCC2.0	none		TCC2.0	F
 *	A1/D6		PA05	TCC0.1	TCC3.3	TCC0.1		TCC0.1	E
 *	A2/D9		PA06	TCC1.0	TCC3.4	TCC1.0		TCC1.0	E
 *	A3/D10		PA07	TCC1.1	TCC3.5	TCC1.1		TCC1.1	E
 *	A4/D3/SCL	PB03	TC6.1	TCC3.3	none		TCC3.3	F
 *	A5/D2/SDA	PB02	TC6.0	TCC3.2	none		TCC3.2	F
 *	A6/D0/RX	PB09	TC4.1	TCC3.7	TC4.1		TC4.1	E
 *	A7/D1/TX	PB08	TC4.0	TCC3.6	TC4.0		TC4.0	E
 *	A8/LIGHT	PA11	TCC1.1	TCC0.3	none		none
 *	A9/TEMPSENSE	PA09	TCC0.1	TCC1.3	none		TCC1.3	F
 *	D4/BUTTONA	PA28		TCC3.7	none		none
 *	D5/BUTTONB	PA14	TC3.0	TCC0.4	none		none
 *	D7/SWITCH	PA15	TC3.1	TCC0.5	TC3.1		none
 *	D8/NEOPIXEL	PB23			none		none
 *	D11/REMOTEOUT	PA23	TC4.1	TCC0.5	TC4.1		none
 *	D12/REMOTEIN	PA12			none		none
 *	D13/LED		PA17	TCC2.1	TCC0.7	none		TCC0.7	F
 *	D27/LISIRQ	PA13	TCC2.1	TCC0.7	none		none
 *	D32/FLASHMISO	PA16	TCC2.0	TCC0.6	none		none
 *	D33/FLASHSCK	PA21		TCC0.7	none		none
 *	D34/FLASHMOSI	PA20		TCC0.6	TCC0.6		none
 *	D35/FLASHCS	PB22		TCC3.0	none		none
 *	I2SCK		PA10	TCC1.0	TCC0.3	none		none
 *	I2SDO		PA08	TCC0.0	TCC1.2	TCC0.0		none
 *	LISSDA		PA00			none		none
 *	LISSCL		PA01			none		none
 *
 * Use this as the 'busy' indicator
 *	TXLED		PA27
 */

/* A0/SPEAKER */
#define AO_SNEK_GPIO_0		(&samd21_port_a)
#define AO_SNEK_PIN_0		2
#define AO_SNEK_TIMER_0		(&samd21_tcc2)
#define AO_SNEK_CHANNEL_0	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_0		0

/* A1/D6 */
#define AO_SNEK_GPIO_1		(&samd21_port_a)
#define AO_SNEK_PIN_1		5
#define AO_SNEK_TIMER_1		(&samd21_tcc0)
#define AO_SNEK_CHANNEL_1	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_1		5

/* A2 */
#define AO_SNEK_GPIO_2		(&samd21_port_a)
#define AO_SNEK_PIN_2		6
#define AO_SNEK_TIMER_2		(&samd21_tcc1)
#define AO_SNEK_CHANNEL_2	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_2		6

/* A3 */
#define AO_SNEK_GPIO_3		(&samd21_port_a)
#define AO_SNEK_PIN_3		7
#define AO_SNEK_TIMER_3		(&samd21_tcc1)
#define AO_SNEK_CHANNEL_3	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_3		7

/* A4 */
#define AO_SNEK_GPIO_4		(&samd21_port_b)
#define AO_SNEK_PIN_4		3
#define AO_SNEK_TIMER_4		(&samd21_tc6)
#define AO_SNEK_CHANNEL_4	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_4		11

/* A5 */
#define AO_SNEK_GPIO_5		(&samd21_port_b)
#define AO_SNEK_PIN_5		2
#define AO_SNEK_TIMER_5		(&samd21_tc6)
#define AO_SNEK_CHANNEL_5	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_5		10

/* A6 */
#define AO_SNEK_GPIO_6		(&samd21_port_b)
#define AO_SNEK_PIN_6		9
#define AO_SNEK_TIMER_6		(&samd21_tc4)
#define AO_SNEK_CHANNEL_6	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_6		3

/* A7 */
#define AO_SNEK_GPIO_7		(&samd21_port_b)
#define AO_SNEK_PIN_7		8
#define AO_SNEK_TIMER_7		(&samd21_tc4)
#define AO_SNEK_CHANNEL_7	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_7		2

/* A8 */
#define AO_SNEK_GPIO_8		(&samd21_port_a)
#define AO_SNEK_PIN_8		11
#define AO_SNEK_TIMER_8		(&samd21_tcc1)
#define AO_SNEK_CHANNEL_8	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_8		19

/* A9/TEMP */
#define AO_SNEK_GPIO_9		(&samd21_port_a)
#define AO_SNEK_PIN_9		9
#define AO_SNEK_TIMER_9		(&samd21_tcc0)
#define AO_SNEK_CHANNEL_9	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_9		17

#define SNEK_THERM_R_DIVIDE	10000.0f
#define SNEK_THERM_R_NOMINAL	10000.0f
#define SNEK_THERM_T_NOMINAL	25.0f
#define SNEK_THERM_B_COEFF	3950.0f
#define SNEK_THERM_HIGH_SIDE	1

/* BUTTONA */
#define AO_SNEK_GPIO_10		(&samd21_port_a)
#define AO_SNEK_PIN_10		28
#define AO_SNEK_TIMER_10	NULL
#define AO_SNEK_CHANNEL_10	0
#define AO_SNEK_ADC_10		0xff
#define AO_SNEK_FLAGS_10	SNEK_PIN_PULL_DOWN

/* BUTTONB */
#define AO_SNEK_GPIO_11		(&samd21_port_a)
#define AO_SNEK_PIN_11		14
#define AO_SNEK_TIMER_11	NULL
#define AO_SNEK_CHANNEL_11	0
#define AO_SNEK_ADC_11		0xff
#define AO_SNEK_FLAGS_11	SNEK_PIN_PULL_DOWN

/* SWITCH */
#define AO_SNEK_GPIO_12		(&samd21_port_a)
#define AO_SNEK_PIN_12		15
#define AO_SNEK_TIMER_12	NULL
#define AO_SNEK_CHANNEL_12	0
#define AO_SNEK_ADC_12		0xff

/* LED */
#define AO_SNEK_GPIO_13		(&samd21_port_a)
#define AO_SNEK_PIN_13		17
#define AO_SNEK_TIMER_13	(&samd21_tcc0)
#define AO_SNEK_CHANNEL_13	AO_SNEK_TIMER_CH(7 & 3, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_13		0xff

/* REMOTEOUT */
#define AO_SNEK_GPIO_14		(&samd21_port_a)
#define AO_SNEK_PIN_14		23
#define AO_SNEK_TIMER_14	NULL
#define AO_SNEK_CHANNEL_14	0
#define AO_SNEK_ADC_14		0xff

/* REMOTEIN */
#define AO_SNEK_GPIO_15		(&samd21_port_a)
#define AO_SNEK_PIN_15		12
#define AO_SNEK_TIMER_15	NULL
#define AO_SNEK_CHANNEL_15	0
#define AO_SNEK_ADC_15		0xff

/* NEOPIXEL */
#define AO_SNEK_GPIO_16		(&samd21_port_b)
#define AO_SNEK_PIN_16		23
#define AO_SNEK_TIMER_16	NULL
#define AO_SNEK_CHANNEL_16	0
#define AO_SNEK_ADC_16		0xff

/* LISIRQ */
#define AO_SNEK_GPIO_17		(&samd21_port_a)
#define AO_SNEK_PIN_17		13
#define AO_SNEK_TIMER_17	NULL
#define AO_SNEK_CHANNEL_17	0
#define AO_SNEK_ADC_17		0xff

/* FLASHMISO */
#define AO_SNEK_GPIO_18		(&samd21_port_a)
#define AO_SNEK_PIN_18		16
#define AO_SNEK_TIMER_18	NULL
#define AO_SNEK_CHANNEL_18	0
#define AO_SNEK_ADC_18		0xff

/* FLASHSCK */
#define AO_SNEK_GPIO_19		(&samd21_port_a)
#define AO_SNEK_PIN_19		21
#define AO_SNEK_TIMER_19	NULL
#define AO_SNEK_CHANNEL_19	0
#define AO_SNEK_ADC_19		0xff

/* FLASHMOSI */
#define AO_SNEK_GPIO_20		(&samd21_port_a)
#define AO_SNEK_PIN_20		20
#define AO_SNEK_TIMER_20	NULL
#define AO_SNEK_CHANNEL_20	0
#define AO_SNEK_ADC_20		0xff

/* FLASHCS */
#define AO_SNEK_GPIO_21		(&samd21_port_b)
#define AO_SNEK_PIN_21		22
#define AO_SNEK_TIMER_21	NULL
#define AO_SNEK_CHANNEL_21	0
#define AO_SNEK_ADC_21		0xff

/* I2SCK */
#define AO_SNEK_GPIO_22		(&samd21_port_a)
#define AO_SNEK_PIN_22		10
#define AO_SNEK_TIMER_22	NULL
#define AO_SNEK_CHANNEL_22	0
#define AO_SNEK_ADC_22		0xff

/* I2SDO */
#define AO_SNEK_GPIO_23		(&samd21_port_a)
#define AO_SNEK_PIN_23		8
#define AO_SNEK_TIMER_23	NULL
#define AO_SNEK_CHANNEL_23	0
#define AO_SNEK_ADC_23		0xff

/* LISSDA */
#define AO_SNEK_GPIO_24		(&samd21_port_a)
#define AO_SNEK_PIN_24		0
#define AO_SNEK_TIMER_24	NULL
#define AO_SNEK_CHANNEL_24	0
#define AO_SNEK_ADC_24		0xff

/* LISSCL */
#define AO_SNEK_GPIO_25		(&samd21_port_a)
#define AO_SNEK_PIN_25		1
#define AO_SNEK_TIMER_25	NULL
#define AO_SNEK_CHANNEL_25	0
#define AO_SNEK_ADC_25		0xff

#define AO_SNEK_NUM_PIN		26

#define LED_0_PORT		(&samd21_port_a)
#define LED_0_PIN		27
#define LED_PIN_TX		0
#define AO_LED_TX		(1 << LED_PIN_TX)
#define AO_LED_PANIC		AO_LED_TX

#define LEDS_AVAILABLE		(AO_LED_TX)

#endif /* _AO_PINS_H_ */
