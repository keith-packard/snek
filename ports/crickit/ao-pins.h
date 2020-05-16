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
 *	Name	Pin	E	F
 *
 * 0	DRIVE1	PB10	TC5.0
 * 1	DRIVE2	PB11	TC5.1
 * 2	DRIVE3	PA12	TCC2.0
 * 3	DRIVE4	PA13	TCC2.1
 * 4	MOTOR1A	PA22	TC4.0
 * 5	MOTOR1B	PA23	TC4.1
 * 6	MOTOR2A	PA19	TC3.1
 * 7	MOTOR2B	PA18	TC3.0
 * 8	SERVO1	PA17		TCC0.7
 * 9	SERVO2	PA16		TCC0.6
 * 10	SERVO3	PA15		TCC0.5
 * 11	SERVO4	PA14		TCC0.4
 * 12	CAP1	PA04
 * 13	CAP2	PA05
 * 14	CAP3	PA06
 * 15	CAP4	PA07
 * 16	SIGNAL1	PA02
 * 17	SIGNAL2	PA03
 * 18	SIGNAL3	PB08
 * 19	SIGNAL4	PB09
 * 20	SIGNAL5	PA11	TCC1.1
 * 21	SIGNAL6	PA10	TCC1.0
 * 22	SIGNAL7	PA09		TCC1.3
 * 23	SIGNAL8	PA08		TCC1.2
 *	LED	PB22
 * 	NEO	PA27
 *	IRQ	PA28
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
#define AO_SNEK_ADC_14	0xff
#define AO_SNEK_ADC_15	0xff

/* DRIVE1 */
#define AO_SNEK_GPIO_0		(&samd21_port_b)
#define AO_SNEK_PIN_0		10
#define AO_SNEK_TIMER_0		(&samd21_tc5)
#define AO_SNEK_CHANNEL_0	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)

/* DRIVE2 */
#define AO_SNEK_GPIO_1		(&samd21_port_b)
#define AO_SNEK_PIN_1		11
#define AO_SNEK_TIMER_1		(&samd21_tc5)
#define AO_SNEK_CHANNEL_1	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)

/* DRIVE3 */
#define AO_SNEK_GPIO_2		(&samd21_port_a)
#define AO_SNEK_PIN_2		12
#define AO_SNEK_TIMER_2		(&samd21_tcc2)
#define AO_SNEK_CHANNEL_2	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)

/* DRIVE4 */
#define AO_SNEK_GPIO_3		(&samd21_port_a)
#define AO_SNEK_PIN_3		13
#define AO_SNEK_TIMER_3		(&samd21_tcc2)
#define AO_SNEK_CHANNEL_3	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)

#define SNEK_DRV8833
#define IS_DRV8833_PWM(p) 	((p) == 4 || (p) == 6)
#define DRV8833_FIND_PWM(dir)	((dir) - 1)
#define IS_DRV8833_DIR(p) 	((p) == 5 || (p) == 7)
#define DRV8833_FIND_DIR(pwm)	((pwm) + 1)

/* MOTOR1A */
#define AO_SNEK_GPIO_4		(&samd21_port_a)
#define AO_SNEK_PIN_4		22
#define AO_SNEK_TIMER_4		(&samd21_tc4)
#define AO_SNEK_CHANNEL_4	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)

/* MOTOR1B */
#define AO_SNEK_GPIO_5		(&samd21_port_a)
#define AO_SNEK_PIN_5		23
#define AO_SNEK_TIMER_5		(&samd21_tc4)
#define AO_SNEK_CHANNEL_5	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)

/* MOTOR2A */
#define AO_SNEK_GPIO_6		(&samd21_port_a)
#define AO_SNEK_PIN_6		19
#define AO_SNEK_TIMER_6		(&samd21_tc3)
#define AO_SNEK_CHANNEL_6	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)

/* MOTOR2B */
#define AO_SNEK_GPIO_7		(&samd21_port_a)
#define AO_SNEK_PIN_7		18
#define AO_SNEK_TIMER_7		(&samd21_tc3)
#define AO_SNEK_CHANNEL_7	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)

/* SERVO1 */
#define AO_SNEK_GPIO_8		(&samd21_port_a)
#define AO_SNEK_PIN_8		17
#define AO_SNEK_TIMER_8		(&samd21_tcc0)
#define AO_SNEK_CHANNEL_8	AO_SNEK_TIMER_CH(7 & 3, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)

/* SERVO2 */
#define AO_SNEK_GPIO_9		(&samd21_port_a)
#define AO_SNEK_PIN_9		16
#define AO_SNEK_TIMER_9		(&samd21_tcc0)
#define AO_SNEK_CHANNEL_9	AO_SNEK_TIMER_CH(6 & 3, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)

/* SERVO3 */
#define AO_SNEK_GPIO_10		(&samd21_port_a)
#define AO_SNEK_PIN_10		15
#define AO_SNEK_TIMER_10	(&samd21_tcc0)
#define AO_SNEK_CHANNEL_10	AO_SNEK_TIMER_CH(5 & 3, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)

/* SERVO4 */
#define AO_SNEK_GPIO_11		(&samd21_port_a)
#define AO_SNEK_PIN_11		14
#define AO_SNEK_TIMER_11	(&samd21_tcc0)
#define AO_SNEK_CHANNEL_11	AO_SNEK_TIMER_CH(4 & 3, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)

/* CAP1 */
#define AO_SNEK_GPIO_12		(&samd21_port_a)
#define AO_SNEK_PIN_12		4
#define AO_SNEK_TIMER_12	NULL
#define AO_SNEK_CHANNEL_12	0

/* CAP2 */
#define AO_SNEK_GPIO_13		(&samd21_port_a)
#define AO_SNEK_PIN_13		5
#define AO_SNEK_TIMER_13	NULL
#define AO_SNEK_CHANNEL_13	0

/* CAP3 */
#define AO_SNEK_GPIO_14		(&samd21_port_a)
#define AO_SNEK_PIN_14		6
#define AO_SNEK_TIMER_14	NULL
#define AO_SNEK_CHANNEL_14	0

/* CAP4 */
#define AO_SNEK_GPIO_15		(&samd21_port_a)
#define AO_SNEK_PIN_15		6
#define AO_SNEK_TIMER_15	NULL
#define AO_SNEK_CHANNEL_15	0

/* SIGNAL1 */
#define AO_SNEK_GPIO_16		(&samd21_port_a)
#define AO_SNEK_PIN_16		2
#define AO_SNEK_TIMER_16	NULL
#define AO_SNEK_CHANNEL_16	0
#define AO_SNEK_ADC_16		0

/* SIGNAL2 */
#define AO_SNEK_GPIO_17		(&samd21_port_a)
#define AO_SNEK_PIN_17		3
#define AO_SNEK_TIMER_17	NULL
#define AO_SNEK_CHANNEL_17	0
#define AO_SNEK_ADC_17		1

/* SIGNAL3 */
#define AO_SNEK_GPIO_18		(&samd21_port_b)
#define AO_SNEK_PIN_18		8
#define AO_SNEK_TIMER_18	NULL
#define AO_SNEK_CHANNEL_18	0
#define AO_SNEK_ADC_18		2

/* SIGNAL4 */
#define AO_SNEK_GPIO_19		(&samd21_port_b)
#define AO_SNEK_PIN_19		9
#define AO_SNEK_TIMER_19	NULL
#define AO_SNEK_CHANNEL_19	0
#define AO_SNEK_ADC_19		3

/* SIGNAL5 */
#define AO_SNEK_GPIO_20		(&samd21_port_a)
#define AO_SNEK_PIN_20		11
#define AO_SNEK_TIMER_20	(&samd21_tcc1)
#define AO_SNEK_CHANNEL_20	AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_20		19

/* SIGNAL6 */
#define AO_SNEK_GPIO_21		(&samd21_port_a)
#define AO_SNEK_PIN_21		10
#define AO_SNEK_TIMER_21	(&samd21_tcc1)
#define AO_SNEK_CHANNEL_21	AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_21		18

/* SIGNAL7 */
#define AO_SNEK_GPIO_22		(&samd21_port_a)
#define AO_SNEK_PIN_22		9
#define AO_SNEK_TIMER_22	(&samd21_tcc1)
#define AO_SNEK_CHANNEL_22	AO_SNEK_TIMER_CH(3, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_22		17

/* SIGNAL8 */
#define AO_SNEK_GPIO_23		(&samd21_port_a)
#define AO_SNEK_PIN_23		8
#define AO_SNEK_TIMER_23	(&samd21_tcc1)
#define AO_SNEK_CHANNEL_23	AO_SNEK_TIMER_CH(2, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_23		16

/* on-board neopixel */
#define AO_SNEK_GPIO_24		(&samd21_port_a)
#define AO_SNEK_PIN_24		27
#define AO_SNEK_TIMER_24	NULL
#define AO_SNEK_CHANNEL_24	0
#define AO_SNEK_ADC_24		0xff

#define NEOPIXEL_RESET			1
#define NEOPIXEL_RESET_POWER_PIN	24

/* off-board neopixel */
#define AO_SNEK_GPIO_25		(&samd21_port_a)
#define AO_SNEK_PIN_25		20
#define AO_SNEK_TIMER_25	NULL
#define AO_SNEK_CHANNEL_25	0
#define AO_SNEK_ADC_25		0xff

#define AO_SNEK_NUM_PIN		26

#define LED_0_PORT		(&samd21_port_b)
#define LED_0_PIN		22
#define LED_PIN_TX		0
#define AO_LED_TX		(1 << LED_PIN_TX)
#define AO_LED_PANIC		AO_LED_TX

#define LEDS_AVAILABLE		(AO_LED_TX)

#endif /* _AO_PINS_H_ */
