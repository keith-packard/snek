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
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * + Pin number +  ZERO Board pin  |  PIN   | Label/Name      | Comments (* is for default peripheral in use)
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | Digital Low      |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 0          | 0 -> RX          |  PA11  |                 | EIC/EXTINT[11] ADC/AIN[19]           PTC/X[3] *SERCOM0/PAD[3]  SERCOM2/PAD[3]  TCC0/WO[3]  TCC1/WO[1]
 * | 1          | 1 <- TX          |  PA10  |                 | EIC/EXTINT[10] ADC/AIN[18]           PTC/X[2] *SERCOM0/PAD[2]                  TCC0/WO[2]  TCC1/WO[0]
 * | 2          | 2                |  PA14  |                 | EIC/EXTINT[14]                                 SERCOM2/PAD[2]  SERCOM4/PAD[2]  TC3/WO[0]   TCC0/WO[4]
 * | 3          | ~3               |  PA09  |                 | EIC/EXTINT[9]  ADC/AIN[17]           PTC/X[1]  SERCOM0/PAD[1]  SERCOM2/PAD[1] *TCC0/WO[1]  TCC1/WO[3]
 * | 4          | ~4               |  PA08  |                 | EIC/NMI        ADC/AIN[16]           PTC/X[0]  SERCOM0/PAD[0]  SERCOM2/PAD[0] *TCC0/WO[0]  TCC1/WO[2]
 * | 5          | ~5               |  PA15  |                 | EIC/EXTINT[15]                                 SERCOM2/PAD[3]  SERCOM4/PAD[3] *TC3/WO[1]   TCC0/WO[5]
 * | 6          | ~6               |  PA20  |                 | EIC/EXTINT[4]                        PTC/X[8]  SERCOM5/PAD[2]  SERCOM3/PAD[2]             *TCC0/WO[6]
 * | 7          | 7                |  PA21  |                 | EIC/EXTINT[5]                        PTC/X[9]  SERCOM5/PAD[3]  SERCOM3/PAD[3]              TCC0/WO[7]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | Digital High     |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 8          | ~8               |  PA06  |                 | EIC/EXTINT[6]  ADC/AIN[6]  AC/AIN[2] PTC/Y[4]  SERCOM0/PAD[2]                 *TCC1/WO[0]
 * | 9          | ~9               |  PA07  |                 | EIC/EXTINT[7]  ADC/AIN[7]  AC/AIN[3] PTC/Y[5]  SERCOM0/PAD[3]                 *TCC1/WO[1]
 * | 10         | ~10              |  PA18  |                 | EIC/EXTINT[2]                        PTC/X[6] +SERCOM1/PAD[2]  SERCOM3/PAD[2] *TC3/WO[0]    TCC0/WO[2]
 * | 11         | ~11              |  PA16  |                 | EIC/EXTINT[0]                        PTC/X[4] +SERCOM1/PAD[0]  SERCOM3/PAD[0] *TCC2/WO[0]   TCC0/WO[6]
 * | 12         | ~12              |  PA19  |                 | EIC/EXTINT[3]                        PTC/X[7] +SERCOM1/PAD[3]  SERCOM3/PAD[3]  TC3/WO[1]   *TCC0/WO[3]
 * | 13         | ~13              |  PA17  | LED             | EIC/EXTINT[1]                        PTC/X[5] +SERCOM1/PAD[1]  SERCOM3/PAD[1] *TCC2/WO[1]   TCC0/WO[7]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | Analog Connector |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 14         | A0               |  PA02  | A0              | EIC/EXTINT[2] *ADC/AIN[0]  DAC/VOUT  PTC/Y[0]
 * | 15         | A1               |  PB08  | A1              | EIC/EXTINT[8] *ADC/AIN[2]            PTC/Y[14] SERCOM4/PAD[0]                  TC4/WO[0]
 * | 16         | A2               |  PB09  | A2              | EIC/EXTINT[9] *ADC/AIN[3]            PTC/Y[15] SERCOM4/PAD[1]                  TC4/WO[1]
 * | 17         | A3               |  PA04  | A3              | EIC/EXTINT[4] *ADC/AIN[4]  AC/AIN[0] PTC/Y[2]  SERCOM0/PAD[0]                  TCC0/WO[0]
 * | 18         | A4               |  PA05  | A4              | EIC/EXTINT[5] *ADC/AIN[5]  AC/AIN[1] PTC/Y[5]  SERCOM0/PAD[1]                  TCC0/WO[1]
 * | 19         | A5               |  PB02  | A5              | EIC/EXTINT[2] *ADC/AIN[10]           PTC/Y[8]  SERCOM5/PAD[0]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | Wire             |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 20         | SDA              |  PA22  | SDA             | EIC/EXTINT[6]                        PTC/X[10] *SERCOM3/PAD[0] SERCOM5/PAD[0] TC4/WO[0] TCC0/WO[4]
 * | 21         | SCL              |  PA23  | SCL             | EIC/EXTINT[7]                        PTC/X[11] *SERCOM3/PAD[1] SERCOM5/PAD[1] TC4/WO[1] TCC0/WO[5]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            |SPI (Legacy ICSP) |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 22         | 1                |  PA12  | MISO            | EIC/EXTINT[12] SERCOM2/PAD[0] *SERCOM4/PAD[0] TCC2/WO[0] TCC0/WO[6]
 * |            | 2                |        | 5V0             |
 * | 23         | 4                |  PB10  | MOSI            | EIC/EXTINT[10]                *SERCOM4/PAD[2] TC5/WO[0]  TCC0/WO[4]
 * | 24         | 3                |  PB11  | SCK             | EIC/EXTINT[11]                *SERCOM4/PAD[3] TC5/WO[1]  TCC0/WO[5]
 * |            | 5                |        | RESET           |
 * |            | 6                |        | GND             |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | LEDs             |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 25         |                  |  PB03  | RX              |
 * | 26         |                  |  PA27  | TX              |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | USB              |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 27         |                  |  PA28  | USB_HOST_ENABLE | EIC/EXTINT[8]
 * | 28         |                  |  PA24  | USB_NEGATIVE    | *USB/DM
 * | 29         |                  |  PA25  | USB_POSITIVE    | *USB/DP
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | EDBG             |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 30         |                  |  PB22  | EDBG_UART TX    | *SERCOM5/PAD[2]
 * | 31         |                  |  PB23  | EDBG_UART RX    | *SERCOM5/PAD[3]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 32         |                  |  PA22  | EDBG_SDA        | Pin 20 (SDA)
 * | 33         |                  |  PA23  | EDBG_SCL        | Pin 21 (SCL)
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 34         |                  |  PA19  | EDBG_MISO       | EIC/EXTINT[3] *SERCOM1/PAD[3] SERCOM3/PAD[3] TC3/WO[1]  TCC0/WO[3]
 * | 35         |                  |  PA16  | EDBG_MOSI       | EIC/EXTINT[0] *SERCOM1/PAD[0] SERCOM3/PAD[0] TCC2/WO[0] TCC0/WO[6]
 * | 36         |                  |  PA18  | EDBG_SS         | EIC/EXTINT[2] *SERCOM1/PAD[2] SERCOM3/PAD[2] TC3/WO[0]  TCC0/WO[2]
 * | 37         |                  |  PA17  | EDBG_SCK        | EIC/EXTINT[1] *SERCOM1/PAD[1] SERCOM3/PAD[1] TCC2/WO[1] TCC0/WO[7]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 38         | ATN              |  PA13  | EDBG_GPIO0      | EIC/EXTINT[13] SERCOM2/PAD[1] SERCOM4/PAD[1] *TCC2/WO[1] TCC0/WO[7]
 * | 39         |                  |  PA21  | EDBG_GPIO1      | Pin 7
 * | 40         |                  |  PA06  | EDBG_GPIO2      | Pin 8
 * | 41         |                  |  PA07  | EDBG_GPIO3      | Pin 9
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            |                  |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | GND              |        |                 |
 * | 42         | AREF             |  PA03  |                 | EIC/EXTINT[3] *[ADC|DAC]/VREFA ADC/AIN[1] PTC/Y[1]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            |32.768KHz Crystal |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            |                  |  PA00  | XIN32           | EIC/EXTINT[0] SERCOM1/PAD[0] TCC2/WO[0]
 * |            |                  |  PA01  | XOUT32          | EIC/EXTINT[1] SERCOM1/PAD[1] TCC2/WO[1]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 */

/* RX/D0 */
#define AO_SNEK_GPIO_0         (&samd21_port_a)
#define AO_SNEK_PIN_0          11
#define AO_SNEK_TIMER_0        NULL
#define AO_SNEK_CHANNEL_0      0
#define AO_SNEK_ADC_0          0xff

/* TX/D1 */
#define AO_SNEK_GPIO_1         (&samd21_port_a)
#define AO_SNEK_PIN_1          10
#define AO_SNEK_TIMER_1        NULL
#define AO_SNEK_CHANNEL_1      0
#define AO_SNEK_ADC_1          0xff

/* D2 */
#define AO_SNEK_GPIO_2         (&samd21_port_a)
#define AO_SNEK_PIN_2          14
#define AO_SNEK_TIMER_2        NULL
#define AO_SNEK_CHANNEL_2      0
#define AO_SNEK_ADC_2          0xff

/* D3 */
#define AO_SNEK_GPIO_3         (&samd21_port_a)
#define AO_SNEK_PIN_3          9
#define AO_SNEK_TIMER_3        (&samd21_tcc0)
#define AO_SNEK_CHANNEL_3      AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_3          0xff

/* D4 */
#define AO_SNEK_GPIO_4         (&samd21_port_a)
#define AO_SNEK_PIN_4          8
#define AO_SNEK_TIMER_4        (&samd21_tcc0)
#define AO_SNEK_CHANNEL_4      AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_4          0xff

/* D5 */
#define AO_SNEK_GPIO_5         (&samd21_port_a)
#define AO_SNEK_PIN_5          15
#define AO_SNEK_TIMER_5        (&samd21_tc3)
#define AO_SNEK_CHANNEL_5      AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_5          0xff

/* D6 */
#define AO_SNEK_GPIO_6         (&samd21_port_a)
#define AO_SNEK_PIN_6          20
#define AO_SNEK_TIMER_6        (&samd21_tcc0)
#define AO_SNEK_CHANNEL_6      AO_SNEK_TIMER_CH(6, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_6          0xff

/* D7 */
#define AO_SNEK_GPIO_7         (&samd21_port_a)
#define AO_SNEK_PIN_7          21
#define AO_SNEK_TIMER_7        NULL
#define AO_SNEK_CHANNEL_7      0
#define AO_SNEK_ADC_7          0xff

/* D8 */
#define AO_SNEK_GPIO_8         (&samd21_port_a)
#define AO_SNEK_PIN_8          6
#define AO_SNEK_TIMER_8        (&samd21_tcc1)
#define AO_SNEK_CHANNEL_8      AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_8          0xff

/* D9 */
#define AO_SNEK_GPIO_9         (&samd21_port_a)
#define AO_SNEK_PIN_9          7
#define AO_SNEK_TIMER_9        (&samd21_tcc1)
#define AO_SNEK_CHANNEL_9      AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_9          0xff

/* D10 */
#define AO_SNEK_GPIO_10        (&samd21_port_a)
#define AO_SNEK_PIN_10         18
#define AO_SNEK_TIMER_10       (&samd21_tc3)
#define AO_SNEK_CHANNEL_10     AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TC)
#define AO_SNEK_ADC_10         0xff

/* D11 */
#define AO_SNEK_GPIO_11        (&samd21_port_a)
#define AO_SNEK_PIN_11         16
#define AO_SNEK_TIMER_11       (&samd21_tcc2)
#define AO_SNEK_CHANNEL_11     AO_SNEK_TIMER_CH(0, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_11         0xff

/* D12 */
#define AO_SNEK_GPIO_12        (&samd21_port_a)
#define AO_SNEK_PIN_12         19
#define AO_SNEK_TIMER_12       (&samd21_tcc0)
#define AO_SNEK_CHANNEL_12     AO_SNEK_TIMER_CH(3, AO_SNEK_TIMER_FUNC_F, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_12         0xff

/* D13 / LED */
#define AO_SNEK_GPIO_13        (&samd21_port_a)
#define AO_SNEK_PIN_13         17
#define AO_SNEK_TIMER_13       (&samd21_tcc2)
#define AO_SNEK_CHANNEL_13     AO_SNEK_TIMER_CH(1, AO_SNEK_TIMER_FUNC_E, AO_SNEK_TIMER_DEV_TCC)
#define AO_SNEK_ADC_13         0xff

/* A0 */
#define AO_SNEK_GPIO_14        (&samd21_port_a)
#define AO_SNEK_PIN_14         2
#define AO_SNEK_TIMER_14       NULL
#define AO_SNEK_CHANNEL_14     0
#define AO_SNEK_ADC_14         0

/* A1 */
#define AO_SNEK_GPIO_15        (&samd21_port_b)
#define AO_SNEK_PIN_15         8
#define AO_SNEK_TIMER_15       NULL
#define AO_SNEK_CHANNEL_15     0
#define AO_SNEK_ADC_15         2

/* A2 */
#define AO_SNEK_GPIO_16        (&samd21_port_b)
#define AO_SNEK_PIN_16         9
#define AO_SNEK_TIMER_16       NULL
#define AO_SNEK_CHANNEL_16     0
#define AO_SNEK_ADC_16         3

/* A3 */
#define AO_SNEK_GPIO_17        (&samd21_port_a)
#define AO_SNEK_PIN_17         4
#define AO_SNEK_TIMER_17       NULL
#define AO_SNEK_CHANNEL_17     0
#define AO_SNEK_ADC_17         4

/* A4 */
#define AO_SNEK_GPIO_18        (&samd21_port_a)
#define AO_SNEK_PIN_18         5
#define AO_SNEK_TIMER_18       NULL
#define AO_SNEK_CHANNEL_18     0
#define AO_SNEK_ADC_18         5

/* A5 */
#define AO_SNEK_GPIO_19        (&samd21_port_b)
#define AO_SNEK_PIN_19         2
#define AO_SNEK_TIMER_19       NULL
#define AO_SNEK_CHANNEL_19     0
#define AO_SNEK_ADC_19         10

/* SDA */
#define AO_SNEK_GPIO_20        (&samd21_port_a)
#define AO_SNEK_PIN_20         22
#define AO_SNEK_TIMER_20       NULL
#define AO_SNEK_CHANNEL_20     0
#define AO_SNEK_ADC_20         0xff

/* SCL */
#define AO_SNEK_GPIO_21        (&samd21_port_a)
#define AO_SNEK_PIN_21         23
#define AO_SNEK_TIMER_21       NULL
#define AO_SNEK_CHANNEL_21     0
#define AO_SNEK_ADC_21         0xff

/* MISO */
#define AO_SNEK_GPIO_22        (&samd21_port_a)
#define AO_SNEK_PIN_22         12
#define AO_SNEK_TIMER_22       NULL
#define AO_SNEK_CHANNEL_22     0
#define AO_SNEK_ADC_22         0xff

/* MOSI */
#define AO_SNEK_GPIO_23        (&samd21_port_b)
#define AO_SNEK_PIN_23         10
#define AO_SNEK_TIMER_23       NULL
#define AO_SNEK_CHANNEL_23     0
#define AO_SNEK_ADC_23         0xff

/* SCK */
#define AO_SNEK_GPIO_24        (&samd21_port_b)
#define AO_SNEK_PIN_24         11
#define AO_SNEK_TIMER_24       NULL
#define AO_SNEK_CHANNEL_24     0
#define AO_SNEK_ADC_24         0xff

/* RX25 */
#define AO_SNEK_GPIO_25        (&samd21_port_b)
#define AO_SNEK_PIN_25         3
#define AO_SNEK_TIMER_25       NULL
#define AO_SNEK_CHANNEL_25     0
#define AO_SNEK_ADC_25         0xff

/* TX26 */
#define AO_SNEK_GPIO_26        (&samd21_port_a)
#define AO_SNEK_PIN_26         27
#define AO_SNEK_TIMER_26       NULL
#define AO_SNEK_CHANNEL_26     0
#define AO_SNEK_ADC_26         0xff

#define AO_SNEK_NUM_PIN		27

#endif /* _AO_PINS_H_ */
