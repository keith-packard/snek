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

#define LED_0_PORT		(&samd21_port_a)
#define LED_0_PIN		17
#define LED_0_INVERT		1
#define LED_PIN_TX		0
#define AO_LED_TX		(1 << LED_PIN_TX)
#define AO_LED_PANIC		(AO_LED_TX)
#define LEDS_AVAILABLE		(AO_LED_TX)


/*
 *	Name	Pin	E	F		tcc0	 tcc1  tcc2  tc3  tc4  tc5
 *						0 1 2 3  0 1   0 1   0 1  0 1  0 1
 *                                              * * * *  * *   * *   * *  * *  * *
 *	A0	PA02            *TCC3_0
 *	A1	PA04    TCC0_0  *TCC3_2		0*
 *	A2	PA10	TCC1_0	TCC0_2		    2*	 0
 *	A3	PA11	TCC1_1	TCC0_3                3*   1
 *	A4	PA08	TCC0_0	TCC1_2          0        2*
 *	A5	PA09	TCC0_1	TCC1_3            1*       3
 *	A6	PB08	TC4_0	*TCC3_6                                   0*
 *	A7	PB09	TC4_1	*TCC3_7                                     1*
 *	A8	PA07	TCC1_1	*TCC3_5                    1*
 *	A9	PA05	TCC0_1	*TCC3_3           1
 *	A10	PA06	TCC1_0	*TCC3_5                  0
 *	LED1	PA18	TC3_0	TCC0_2              2                0*
 *	LED2	PA19	TC3_1	TCC0_3                3                1*
 *
 * +------------+------------------+--------+--------------------------------------------------------------------------------------------------------
 * + Pin number +  Xiao Board pin  |  PIN   | Comments (* is for default peripheral in use)
 * +------------+------------------+--------+--------------------------------------------------------------------------------------------------------
 * | 0          | A0/D0            |  PA02  | EIC/EXTINT[2] *ADC/AIN[0]  DAC/VOUT  PTC/Y[0]
 * | 1          | A1/D1            |  PA04  | EIC/EXTINT[4] *ADC/AIN[4]  AC/AIN[0] PTC/Y[2]  SERCOM0/PAD[0]                  TCC0/WO[0]
 * | 2          | A2/D2            |  PA10  | EIC/EXTINT[10] ADC/AIN[18]           PTC/X[2] *SERCOM0/PAD[2]                  TCC0/WO[2]  TCC1/WO[0]
 * | 3          | A3/D3            |  PA11  | EIC/EXTINT[11] ADC/AIN[19]           PTC/X[3] *SERCOM0/PAD[3]  SERCOM2/PAD[3]  TCC0/WO[3]  TCC1/WO[1]
 * | 4          | A4/D4            |  PA08  | EIC/NMI        ADC/AIN[16]           PTC/X[0]  SERCOM0/PAD[0]  SERCOM2/PAD[0] *TCC0/WO[0]  TCC1/WO[2]
 * | 5          | A5/D5            |  PA09  | EIC/EXTINT[9]  ADC/AIN[17]           PTC/X[1]  SERCOM0/PAD[1]  SERCOM2/PAD[1] *TCC0/WO[1]  TCC1/WO[3]
 * | 6          | A6/D6            |  PB08  | EIC/EXTINT[8] *ADC/AIN[2]            PTC/Y[14] SERCOM4/PAD[0]                  TC4/WO[0]
 * | 7          | A7/D7            |  PB09  | EIC/EXTINT[9] *ADC/AIN[3]            PTC/Y[15] SERCOM4/PAD[1]                  TC4/WO[1]
 * | 8          | A8/D8            |  PA07  | EIC/EXTINT[7]  ADC/AIN[7]  AC/AIN[3] PTC/Y[5]  SERCOM0/PAD[3]                 *TCC1/WO[1]
 * | 9          | A9/D9            |  PA05  | EIC/EXTINT[5] *ADC/AIN[5]  AC/AIN[1] PTC/Y[5]  SERCOM0/PAD[1]                  TCC0/WO[1]
 * | 10         | A10/D10          |  PA06  | EIC/EXTINT[6]  ADC/AIN[6]  AC/AIN[2] PTC/Y[4]  SERCOM0/PAD[2]                 *TCC1/WO[0]
 * | 11         | LED1             |  PA18  | EIC/EXTINT[2]                        PTC/X[6] +SERCOM1/PAD[2]  SERCOM3/PAD[2] *TC3/WO[0]    TCC0/WO[2]
 * | 12         | LED2             |  PA19  | EIC/EXTINT[3]                        PTC/X[7] +SERCOM1/PAD[3]  SERCOM3/PAD[3]  TC3/WO[1]   *TCC0/WO[3]
 */

/* A0/D0 PA02 */
#define AO_SNEK_GPIO_0         (&samd21_port_a)
#define AO_SNEK_PIN_0          2
#define AO_SNEK_TIMER_0        NULL
#define AO_SNEK_CHANNEL_0      0
#define AO_SNEK_ADC_0          0

/* A1/D1 PA04 */
#define AO_SNEK_GPIO_1         (&samd21_port_a)
#define AO_SNEK_PIN_1          4
#define AO_SNEK_TIMER_1        (&samd21_tcc0)
#define AO_SNEK_CHANNEL_1      AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_1          4

/* A2/D2 PA10 */
#define AO_SNEK_GPIO_2         (&samd21_port_a)
#define AO_SNEK_PIN_2          10
#define AO_SNEK_TIMER_2        (&samd21_tcc0)
#define AO_SNEK_CHANNEL_2      AO_SNEK_TIMER_CH(2, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_2          18

/* A3/D3 PA11 */
#define AO_SNEK_GPIO_3         (&samd21_port_a)
#define AO_SNEK_PIN_3          11
#define AO_SNEK_TIMER_3        (&samd21_tcc0)
#define AO_SNEK_CHANNEL_3      AO_SNEK_TIMER_CH(3, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_3          19

/* A4/D4 PA08 SDA */
#define AO_SNEK_GPIO_4         (&samd21_port_a)
#define AO_SNEK_PIN_4          8
#define AO_SNEK_TIMER_4        (&samd21_tcc1)
#define AO_SNEK_CHANNEL_4      AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_4          16

/* A5/D5 PA09 SCL */
#define AO_SNEK_GPIO_5         (&samd21_port_a)
#define AO_SNEK_PIN_5          9
#define AO_SNEK_TIMER_5        (&samd21_tcc0)
#define AO_SNEK_CHANNEL_5      AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_5          17

/* A6/D6 PB08 TX */
#define AO_SNEK_GPIO_6         (&samd21_port_b)
#define AO_SNEK_PIN_6          8
#define AO_SNEK_TIMER_6        (&samd21_tc4)
#define AO_SNEK_CHANNEL_6      AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_6          2

/* A7/D7 PB09 RX */
#define AO_SNEK_GPIO_7         (&samd21_port_b)
#define AO_SNEK_PIN_7          9
#define AO_SNEK_TIMER_7        (&samd21_tc4)
#define AO_SNEK_CHANNEL_7      AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_7          3

/* A8/D8 PA07 SCK */
#define AO_SNEK_GPIO_8         (&samd21_port_a)
#define AO_SNEK_PIN_8          7
#define AO_SNEK_TIMER_8        (&samd21_tcc1)
#define AO_SNEK_CHANNEL_8      AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_8          7

/* A9/D9 PA05 MISO */
#define AO_SNEK_GPIO_9         (&samd21_port_a)
#define AO_SNEK_PIN_9          5
#define AO_SNEK_TIMER_9        NULL
#define AO_SNEK_CHANNEL_9      0
#define AO_SNEK_ADC_9          5

/* A10/D10 PA06 MOSI */
#define AO_SNEK_GPIO_10        (&samd21_port_a)
#define AO_SNEK_PIN_10         6
#define AO_SNEK_TIMER_10       NULL
#define AO_SNEK_CHANNEL_10     0
#define AO_SNEK_ADC_10         6

/* RX_LED PA18 */
#define AO_SNEK_GPIO_11        (&samd21_port_a)
#define AO_SNEK_PIN_11         18
#define AO_SNEK_TIMER_11       (&samd21_tc3)
#define AO_SNEK_CHANNEL_11     AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_11         0xff
#define AO_SNEK_FLAGS_11       SNEK_PIN_INVERT

/* TX_LED PA19 */
#define AO_SNEK_GPIO_12        (&samd21_port_a)
#define AO_SNEK_PIN_12         19
#define AO_SNEK_TIMER_12       (&samd21_tc3)
#define AO_SNEK_CHANNEL_12     AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_12         0xff
#define AO_SNEK_FLAGS_12       SNEK_PIN_INVERT

#define AO_SNEK_NUM_PIN		13

#endif /* _AO_PINS_H_ */
