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

#ifndef _SAMD21_H_
#define _SAMD21_H_

#include <stdint.h>

typedef volatile uint64_t	vuint64_t;
typedef volatile uint32_t	vuint32_t;
typedef volatile void *		vvoid_t;
typedef volatile uint16_t	vuint16_t;
typedef volatile uint8_t	vuint8_t;

struct samd21_pac {
	vuint32_t	wpclr;
	vuint32_t	wpset;
};

extern struct samd21_pac samd21_pac0;
extern struct samd21_pac samd21_pac1;
extern struct samd21_pac samd21_pac2;

#define samd21_pac0 (*(struct samd21_pac *) 0x40000000)
#define samd21_pac1 (*(struct samd21_pac *) 0x41000000)
#define samd21_pac2 (*(struct samd21_pac *) 0x42000000)

struct samd21_gclk {
	vuint8_t	ctrl;
	vuint8_t	status;
	vuint16_t	clkctrl;
	vuint32_t	genctrl;
	vuint32_t	gendiv;
};

extern struct samd21_gclk samd21_gclk;

#define samd21_gclk	(*(struct samd21_gclk *) 0x40000c00)

#define SAMD21_GCLK_CTRL_SWRST		0

#define SAMD21_GCLK_STATUS_SYNCBUSY	7

#define SAMD21_GCLK_CLKCTRL_ID		0
#define  SAMD21_GCLK_CLKCTRL_ID_DFLL48M_REF		0
#define  SAMD21_GCLK_CLKCTRL_ID_DPLL			1
#define  SAMD21_GCLK_CLKCTRL_ID_DPLL_32K		2
#define  SAMD21_GCLK_CLKCTRL_ID_WDT			3
#define  SAMD21_GCLK_CLKCTRL_ID_RTC			4
#define  SAMD21_GCLK_CLKCTRL_ID_EIC			5
#define  SAMD21_GCLK_CLKCTRL_ID_USB			6
#define  SAMD21_GCLK_CLKCTRL_ID_EVSYS_CHANNEL_0		0x07
#define  SAMD21_GCLK_CLKCTRL_ID_EVSYS_CHANNEL_1		0x08
#define  SAMD21_GCLK_CLKCTRL_ID_EVSYS_CHANNEL_2		0x09
#define  SAMD21_GCLK_CLKCTRL_ID_EVSYS_CHANNEL_3		0x0a
#define  SAMD21_GCLK_CLKCTRL_ID_EVSYS_CHANNEL_4		0x0b
#define  SAMD21_GCLK_CLKCTRL_ID_EVSYS_CHANNEL_5		0x0c
#define  SAMD21_GCLK_CLKCTRL_ID_EVSYS_CHANNEL_6		0x0d
#define  SAMD21_GCLK_CLKCTRL_ID_EVSYS_CHANNEL_7		0x0e
#define  SAMD21_GCLK_CLKCTRL_ID_EVSYS_CHANNEL_8		0e0f
#define  SAMD21_GCLK_CLKCTRL_ID_EVSYS_CHANNEL_9		0x10
#define  SAMD21_GCLK_CLKCTRL_ID_EVSYS_CHANNEL_10	0x11
#define  SAMD21_GCLK_CLKCTRL_ID_EVSYS_CHANNEL_11	0x12
#define  SAMD21_GCLK_CLKCTRL_ID_SERCOMx_SLOW		0x13
#define  SAMD21_GCLK_CLKCTRL_ID_SERCOM0_CORE		0x14
#define  SAMD21_GCLK_CLKCTRL_ID_SERCOM1_CORE		0x15
#define  SAMD21_GCLK_CLKCTRL_ID_SERCOM2_CORE		0x16
#define  SAMD21_GCLK_CLKCTRL_ID_SERCOM3_CORE		0x17
#define  SAMD21_GCLK_CLKCTRL_ID_SERCOM4_CORE		0x18
#define  SAMD21_GCLK_CLKCTRL_ID_SERCOM5_CORE		0x19
#define  SAMD21_GCLK_CLKCTRL_ID_TCC0_TCC1		0x1a
#define  SAMD21_GCLK_CLKCTRL_ID_TCC2_TC3		0x1b
#define  SAMD21_GCLK_CLKCTRL_ID_TC4_TC5			0x1c
#define  SAMD21_GCLK_CLKCTRL_ID_TC6_TC7			0x1d
#define  SAMD21_GCLK_CLKCTRL_ID_ADC			0x1e
#define  SAMD21_GCLK_CLKCTRL_ID_AC_DIG			0x1f
#define  SAMD21_GCLK_CLKCTRL_ID_AC_ANA			0x20
#define  SAMD21_GCLK_CLKCTRL_ID_DAC			0x21
#define  SAMD21_GCLK_CLKCTRL_ID_PTC			0x22
#define  SAMD21_GCLK_CLKCTRL_ID_I2S_0			0x23
#define  SAMD21_GCLK_CLKCTRL_ID_I2S_1			0x24
#define  SAMD21_GCLK_CLKCTRL_ID_TCC3			0x25

#define SAMD21_GCLK_CLKCTRL_GEN		8
#define SAMD21_GCLK_CLKCTRL_CLKEN	14
#define SAMD21_GCLK_CLKCTRL_WRTLOCK	15

#define SAMD21_GCLK_GENCTRL_ID		0
#define SAMD21_GCLK_GENCTRL_SRC		8
#define  SAMD21_GCLK_GENCTRL_SRC_XOSC		0
#define  SAMD21_GCLK_GENCTRL_SRC_GCLKIN		1
#define  SAMD21_GCLK_GENCTRL_SRC_GCLKGEN1	2
#define  SAMD21_GCLK_GENCTRL_SRC_OSCULP32K	3
#define  SAMD21_GCLK_GENCTRL_SRC_OSC32K		4
#define  SAMD21_GCLK_GENCTRL_SRC_XOSC32K	5
#define  SAMD21_GCLK_GENCTRL_SRC_OSC8M		6
#define  SAMD21_GCLK_GENCTRL_SRC_DFLL48M	7
#define  SAMD21_GCLK_GENCTRL_SRC_FDPLL96M	8

#define SAMD21_GCLK_GENCTRL_GENEN	16
#define SAMD21_GCLK_GENCTRL_IDC		17
#define SAMD21_GCLK_GENCTRL_OOV		18
#define SAMD21_GCLK_GENCTRL_OE		19
#define SAMD21_GCLK_GENCTRL_DIVSEL	20
#define SAMD21_GCLK_GENCTRL_RUNSTDBY	21

#define SAMD21_GCLK_GENDIV_ID		0
#define SAMD21_GCLK_GENDIV_DIV		8

struct samd21_pm {
	vuint8_t	ctrl;
	vuint8_t	sleep;
	vuint8_t	reserved_02;
	vuint8_t	reserved_03;
	vuint32_t	reserved_04;
	vuint8_t	cpusel;
	vuint8_t	apbasel;
	vuint8_t	apbbsel;
	vuint8_t	apbcsel;
	vuint32_t	reserved_0c;

	vuint32_t	reserved_10;
	vuint32_t	ahbmask;
	vuint32_t	apbamask;
	vuint32_t	apbbmask;

	vuint32_t	apbcmask;
	vuint32_t	reserved_24;
	vuint32_t	reserved_28;
	vuint32_t	reserved_2c;

	vuint32_t	reserved_30;
	vuint8_t	intenclr;
	vuint8_t	intelset;
	vuint8_t	intflag;
	vuint8_t	reserved_37;
	vuint8_t	rcause;
};

extern struct samd21_pm samd21_pm;

#define samd21_pm	(*(struct samd21_pm *) 0x40000400)

#define SAMD21_PM_CPUSEL_CPUDIV		0
#define SAMD21_PM_APBASEL_APBADIV	0
#define SAMD21_PM_APBBSEL_APBBDIV	0
#define SAMD21_PM_APBCSEL_APBCDIV	0

#define SAMD21_PM_APBAMASK_PAC0		0
#define SAMD21_PM_APBAMASK_PM		1
#define SAMD21_PM_APBAMASK_SYSCTRL	2
#define SAMD21_PM_APBAMASK_GCLK		3
#define SAMD21_PM_APBAMASK_WDT		4
#define SAMD21_PM_APBAMASK_RTC		5
#define SAMD21_PM_APBAMASK_EIC		6

#define SAMD21_PM_AHBMASK_HPB0		0
#define SAMD21_PM_AHBMASK_HPB1		1
#define SAMD21_PM_AHBMASK_HPB2		2
#define SAMD21_PM_AHBMASK_DSU		3
#define SAMD21_PM_AHBMASK_NVMCTRL	4
#define SAMD21_PM_AHBMASK_DMAC		5
#define SAMD21_PM_AHBMASK_USB		6

#define SAMD21_PM_APBBMASK_PAC1		0
#define SAMD21_PM_APBBMASK_DSU		1
#define SAMD21_PM_APBBMASK_NVMCTRL	2
#define SAMD21_PM_APBBMASK_PORT		3
#define SAMD21_PM_APBBMASK_DMAC		4
#define SAMD21_PM_APBBMASK_USB		5

#define SAMD21_PM_APBCMASK_PAC2		0
#define SAMD21_PM_APBCMASK_EVSYS	1
#define SAMD21_PM_APBCMASK_SERCOM0	2
#define SAMD21_PM_APBCMASK_SERCOM1	3
#define SAMD21_PM_APBCMASK_SERCOM2	4
#define SAMD21_PM_APBCMASK_SERCOM3	5
#define SAMD21_PM_APBCMASK_SERCOM4	6
#define SAMD21_PM_APBCMASK_SERCOM5	7
#define SAMD21_PM_APBCMASK_TCC0		8
#define SAMD21_PM_APBCMASK_TCC1		9
#define SAMD21_PM_APBCMASK_TCC2		10
#define SAMD21_PM_APBCMASK_TC3		11
#define SAMD21_PM_APBCMASK_TC4		12
#define SAMD21_PM_APBCMASK_TC5		13
#define SAMD21_PM_APBCMASK_TC6		14
#define SAMD21_PM_APBCMASK_TC7		15
#define SAMD21_PM_APBCMASK_ADC		16
#define SAMD21_PM_APBCMASK_AC		17
#define SAMD21_PM_APBCMASK_DAC		18
#define SAMD21_PM_APBCMASK_PTC		19
#define SAMD21_PM_APBCMASK_I2S		20
#define SAMD21_PM_APBCMASK_AC1		21
#define SAMD21_PM_APBCMASK_TCC3		24

struct samd21_sysctrl {
	vuint32_t	intenclr;
	vuint32_t	intenset;
	vuint32_t	intflag;
	vuint32_t	pclksr;

	vuint32_t	xosc;
	vuint32_t	xosc32k;
	vuint32_t	osc32k;
	vuint32_t	osculp32k;

	vuint32_t	osc8m;
	vuint32_t	dfllctrl;
	vuint32_t	dfllval;
	vuint32_t	dfllmul;

	vuint32_t	dfllsync;
	vuint32_t	bod33;
	vuint32_t	reserved_38;
	vuint32_t	vreg;

	vuint32_t	vref;
	vuint32_t	dpllctrla;
	vuint32_t	dpllratio;
	vuint32_t	dpllctrlb;

	vuint32_t	dpllstatus;
};

extern struct samd21_sysctrl samd21_sysctrl;

#define samd21_sysctrl (*(struct samd21_sysctrl *) 0x40000800)

#define SAMD21_SYSCTRL_PCLKSR_XOSCRDY		0
#define SAMD21_SYSCTRL_PCLKSR_XOSC32KRDY	1
#define SAMD21_SYSCTRL_PCLKSR_OSC32KRDY		2
#define SAMD21_SYSCTRL_PCLKSR_OSC8MRDY		3
#define SAMD21_SYSCTRL_PCLKSR_DFLLRDY		4
#define SAMD21_SYSCTRL_PCLKSR_DFLLOOB		5
#define SAMD21_SYSCTRL_PCLKSR_DFLLLCKF		6
#define SAMD21_SYSCTRL_PCLKSR_DFLLLCKC		7
#define SAMD21_SYSCTRL_PCLKSR_DFLLRCS		8
#define SAMD21_SYSCTRL_PCLKSR_BOD33RDY		9
#define SAMD21_SYSCTRL_PCLKSR_BOD33DET		10
#define SAMD21_SYSCTRL_PCLKSR_B33SRDY		11
#define SAMD21_SYSCTRL_PCLKSR_DBPLLLCKR		15
#define SAMD21_SYSCTRL_PCLKSR_DPLLLCKF		16
#define SAMD21_SYSCTRL_PCLKSR_DPLLTO		17

#define SAMD21_SYSCTRL_XOSC_ENABLE		1
#define SAMD21_SYSCTRL_XOSC_XTALEN		2
#define SAMD21_SYSCTRL_XOSC_RUNSTDBY		6
#define SAMD21_SYSCTRL_XOSC_ONDEMAND		7
#define SAMD21_SYSCTRL_XOSC_GAIN		8
#define  SAMD21_SYSCTRL_XOSC_GAIN_2MHz			0
#define  SAMD21_SYSCTRL_XOSC_GAIN_4MHz			1
#define  SAMD21_SYSCTRL_XOSC_GAIN_8MHz			2
#define  SAMD21_SYSCTRL_XOSC_GAIN_16MHz			3
#define  SAMD21_SYSCTRL_XOSC_GAIN_30MHz			4
#define SAMD21_SYSCTRL_XOSC_AMPGC		11
#define SAMD21_SYSCTRL_XOSC_STARTUP		12
#define  SAMD21_SYSCTRL_XOSC_STARTUP_1			0
#define  SAMD21_SYSCTRL_XOSC_STARTUP_2 			1
#define  SAMD21_SYSCTRL_XOSC_STARTUP_4			2
#define  SAMD21_SYSCTRL_XOSC_STARTUP_8			3
#define  SAMD21_SYSCTRL_XOSC_STARTUP_16			4
#define  SAMD21_SYSCTRL_XOSC_STARTUP_32			5
#define  SAMD21_SYSCTRL_XOSC_STARTUP_64			6
#define  SAMD21_SYSCTRL_XOSC_STARTUP_128	       	7
#define  SAMD21_SYSCTRL_XOSC_STARTUP_256		8
#define  SAMD21_SYSCTRL_XOSC_STARTUP_512		9
#define  SAMD21_SYSCTRL_XOSC_STARTUP_1024		10
#define  SAMD21_SYSCTRL_XOSC_STARTUP_2048		11
#define  SAMD21_SYSCTRL_XOSC_STARTUP_4096		12
#define  SAMD21_SYSCTRL_XOSC_STARTUP_8192		13
#define  SAMD21_SYSCTRL_XOSC_STARTUP_16384		14
#define  SAMD21_SYSCTRL_XOSC_STARTUP_32768		15

#define SAMD21_SYSCTRL_XOSC32K_ENABLE	1
#define SAMD21_SYSCTRL_XOSC32K_XTALEN	2
#define SAMD21_SYSCTRL_XOSC32K_EN32K	3
#define SAMD21_SYSCTRL_XOSC32K_AAMPEN	5
#define SAMD21_SYSCTRL_XOSC32K_RUNSTDBY	6
#define SAMD21_SYSCTRL_XOSC32K_ONDEMAND	7
#define SAMD21_SYSCTRL_XOSC32K_STARTUP	8
#define SAMD21_SYSCTRL_XOSC32K_WRTLOCK	12

#define SAMD21_SYSCTRL_OSC8M_ENABLE	1
#define SAMD21_SYSCTRL_OSC8M_RUNSTDBY	6
#define SAMD21_SYSCTRL_OSC8M_ONDEMAND	7
#define SAMD21_SYSCTRL_OSC8M_PRESC	8
#define  SAMD21_SYSCTRL_OSC8M_PRESC_1		0
#define  SAMD21_SYSCTRL_OSC8M_PRESC_2		1
#define  SAMD21_SYSCTRL_OSC8M_PRESC_4		2
#define  SAMD21_SYSCTRL_OSC8M_PRESC_8		3
#define  SAMD21_SYSCTRL_OSC8M_PRESC_MASK	3
#define SAMD21_SYSCTRL_OSC8M_CALIB	16
#define SAMD21_SYSCTRL_OSC8M_FRANGE	30
#define  SAMD21_SYSCTRL_OSC8M_FRANGE_4_6	0
#define  SAMD21_SYSCTRL_OSC8M_FRANGE_6_8	1
#define  SAMD21_SYSCTRL_OSC8M_FRANGE_8_11	2
#define  SAMD21_SYSCTRL_OSC8M_FRANGE_11_15	3

#define SAMD21_SYSCTRL_DFLLCTRL_ENABLE		1
#define SAMD21_SYSCTRL_DFLLCTRL_MODE		2
#define SAMD21_SYSCTRL_DFLLCTRL_STABLE		3
#define SAMD21_SYSCTRL_DFLLCTRL_LLAW		4
#define SAMD21_SYSCTRL_DFLLCTRL_USBCRM		5
#define SAMD21_SYSCTRL_DFLLCTRL_RUNSTDBY	6
#define SAMD21_SYSCTRL_DFLLCTRL_ONDEMAND	7
#define SAMD21_SYSCTRL_DFLLCTRL_CCDIS		8
#define SAMD21_SYSCTRL_DFLLCTRL_QLDIS		9
#define SAMD21_SYSCTRL_DFLLCTRL_BPLCKC		10
#define SAMD21_SYSCTRL_DFLLCTRL_WAITLOCK	11

#define SAMD21_SYSCTRL_DFLLVAL_FINE		0
#define SAMD21_SYSCTRL_DFLLVAL_COARSE		10
#define SAMD21_SYSCTRL_DFLLVAL_DIFF		16

#define SAMD21_SYSCTRL_DFLLMUL_MUL		0
#define SAMD21_SYSCTRL_DFLLMUL_FSTEP		16
#define SAMD21_SYSCTRL_DFLLMUL_CSTEP		26

#define SAMD21_SYSCTRL_DFLLSYNC_READREQ		7

#define SAMD21_SYSCTRL_DPLLCTRLA_ENABLE		1
#define SAMD21_SYSCTRL_DPLLCTRLA_RUNSTDBY	6
#define SAMD21_SYSCTRL_DPLLCTRLA_ONDEMAND	7

#define SAMD21_SYSCTRL_DPLLRATIO_LDR		0
#define SAMD21_SYSCTRL_DPLLRATIO_LDRFRAC	0

#define SAMD21_SYSCTRL_DPLLCTRLB_FILTER		0
#define  SAMD21_SYSCTRL_DPLLCTRLB_FILTER_DEFAULT	0
#define  SAMD21_SYSCTRL_DPLLCTRLB_FILTER_LBFILT		1
#define  SAMD21_SYSCTRL_DPLLCTRLB_FILTER_HBFILT		2
#define  SAMD21_SYSCTRL_DPLLCTRLB_FILTER_HDFILT		3
#define SAMD21_SYSCTRL_DPLLCTRLB_LPEN		2
#define SAMD21_SYSCTRL_DPLLCTRLB_WUF		3
#define SAMD21_SYSCTRL_DPLLCTRLB_REFCLK		4
#define  SAMD21_SYSCTRL_DPLLCTRLB_REFCLK_XOSC32		0
#define  SAMD21_SYSCTRL_DPLLCTRLB_REFCLK_XOSC		1
#define  SAMD21_SYSCTRL_DPLLCTRLB_REFCLK_GCLK_DPLL	2
#define SAMD21_SYSCTRL_DPLLCTRLB_LTIME		8
#define  SAMD21_SYSCTRL_DPLLCTRLB_LTIME_DEFAULT		0
#define  SAMD21_SYSCTRL_DPLLCTRLB_LTIME_8MS		4
#define  SAMD21_SYSCTRL_DPLLCTRLB_LTIME_9MS		5
#define  SAMD21_SYSCTRL_DPLLCTRLB_LTIME_10MS		6
#define  SAMD21_SYSCTRL_DPLLCTRLB_LTIME_11MS		7
#define SAMD21_SYSCTRL_DPLLCTRLB_LBYPASS	12
#define SAMD21_SYSCTRL_DPLLCTRLB_DIV		16

#define SAMD21_SYSCTRL_DPLLSTATUS_LOCK		0
#define SAMD21_SYSCTRL_DPLLSTATUS_CLKRDY	1
#define SAMD21_SYSCTRL_DPLLSTATUS_ENABLE	2
#define SAMD21_SYSCTRL_DPLLSTATUS_DIV		3

struct samd21_dmac {
	vuint16_t	ctrl;
	vuint16_t	crcctrl;
	vuint32_t	crcdatain;
	vuint32_t	crcchksum;
	vuint8_t	crcstatus;
	vuint8_t	dbgctrl;
	vuint8_t	qosctrl;
	uint8_t		reserved_0f;

	vuint32_t	swtrigctrl;
	vuint32_t	prictrl0;
	uint32_t	reserved_18;
	uint32_t	reserved_1c;

	vuint16_t	intpend;
	uint16_t	reserved_22;
	vuint32_t	intstatus;
	vuint32_t	busych;
	vuint32_t	pendch;

	vuint32_t	active;
	vuint32_t	baseaddr;
	vuint32_t	wrbaddr;
	uint16_t	reserved_3c;
	uint8_t		reserved_3e;
	vuint8_t	chid;

	vuint8_t	chctrla;
	uint8_t		reserved_41;
	uint16_t	reserved_42;
	vuint32_t	chctrlb;
	uint32_t	reserved_48;
	vuint8_t	chintenclr;
	vuint8_t	chintenset;
	vuint8_t	chintflag;
	vuint8_t	chstatus;
};

extern struct samd21_dmac samd21_dmac;

#define samd21_dmac (*(struct samd21_dmac *) 0x41004800)

struct samd21_dmac_desc {
	vuint16_t	btctrl;
	vuint16_t	btcnt;
	vuint32_t	srcaddr;
	vuint32_t	dstaddr;
	vuint32_t	descaddr;
} __attribute__((aligned(8)));

#define SAMD21_DMAC_NCHAN		12

#define SAMD21_DMAC_CTRL_SWRST		0
#define SAMD21_DMAC_CTRL_DMAENABLE	1
#define SAMD21_DMAC_CTRL_CRCENABLE	2
#define SAMD21_DMAC_CTRL_LVLEN(x)	(8 + (x))

#define SAMD21_DMAC_QOSCTRL_WRBQOS	0
#define SAMD21_DMAC_QOSCTRL_FQOS	2
#define SAMD21_DMAC_QOSCTRL_DQOS	4

#define SAMD21_DMAC_QOSCTRL_DISABLE	0
#define SAMD21_DMAC_QOSCTRL_LOW		1
#define SAMD21_DMAC_QOSCTRL_MEDIUM	2
#define SAMD21_DMAC_QOSCTRL_HIGH	3

#define SAMD21_DMAC_SWTRIGCTRL_SWTRIG(n)	(0 + (n))

#define SAMD21_DMAC_PRICTRL0_LVLPRI0	0
#define SAMD21_DMAC_PRICTRL0_RRLVLEN0	7
#define SAMD21_DMAC_PRICTRL0_LVLPRI1	8
#define SAMD21_DMAC_PRICTRL0_RRLVLEN1	15
#define SAMD21_DMAC_PRICTRL0_LVLPRI2	16
#define SAMD21_DMAC_PRICTRL0_RRLVLEN2	23
#define SAMD21_DMAC_PRICTRL0_LVLPRI3	24
#define SAMD21_DMAC_PRICTRL0_RRLVLEN3	31

#define SAMD21_DMAC_INTPEND_ID		0
#define  SAMD21_DMAC_INTPEND_ID_MASK		0xf
#define SAMD21_DMAC_INTPEND_TERR	8
#define SAMD21_DMAC_INTPEND_TCMPL	9
#define SAMD21_DMAC_INTPEND_SUSP	10
#define SAMD21_DMAC_INTPEND_FERR	13
#define SAMD21_DMAC_INTPEND_BUSY	14
#define SAMD21_DMAC_INTPEND_PEND	15

#define SAMD21_DMAC_INTSTATUS_CHINT(n)	(0 + (n))

#define SAMD21_DMAC_BUSYCH_BUSYCH(n)	(0 + (n))

#define SAMD21_DMAC_PENDCH_PENDCH(n)	(0 + (n))

#define SAMD21_DMAC_ACTIVE_LVLEX(x)	(0 + (x))
#define SAMD21_DMAC_ACTIVE_ID		8
#define SAMD21_DMAC_ACTIVE_ABUSY	15
#define SAMD21_DMAC_ACTIVE_BTCNT	16

#define SAMD21_DMAC_CHCTRLA_SWRST	0
#define SAMD21_DMAC_CHCTRLA_ENABLE	1

#define SAMD21_DMAC_CHCTRLB_EVACT	0
#define  SAMD21_DMAC_CHCTRLB_EVACT_NOACT	0
#define  SAMD21_DMAC_CHCTRLB_EVACT_TRIG		1
#define  SAMD21_DMAC_CHCTRLB_EVACT_CTRIG	2
#define  SAMD21_DMAC_CHCTRLB_EVACT_CBLOCK	3
#define  SAMD21_DMAC_CHCTRLB_EVACT_SUSPEND	4
#define  SAMD21_DMAC_CHCTRLB_EVACT_RESUME	5
#define  SAMD21_DMAC_CHCTRLB_EVACT_SSKIP	6

#define SAMD21_DMAC_CHCTRLB_EVIE	3
#define SAMD21_DMAC_CHCTRLB_EVOE	4
#define SAMD21_DMAC_CHCTRLB_LVL		5
#define SAMD21_DMAC_CHCTRLB_TRIGSRC	8
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_DISABLE	0x00
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_SERCOM_RX(n)	(0x01 + (n) * 2)
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_SERCOM_TX(n)	(0x02 + (n) * 2)
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC0_OVF	0x0d
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC0_MC0	0x0e
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC0_MC1	0x0f
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC0_MC2	0x10
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC0_MC3	0x11
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC1_OVF	0x12
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC1_MC0	0x13
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC1_MC1	0x14
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC2_OVF	0x15
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC2_MC0	0x16
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC2_MC1	0x17
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC3_OVF	0x18
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC3_MC0	0x19
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC3_MC1	0x1a
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC4_OVF	0x1b
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC4_MC0	0x1c
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC4_MC1	0x1d
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC5_OVF	0x1e
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC5_MC0	0x1f
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC5_MC1	0x20
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC6_OVF	0x21
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC6_MC0	0x22
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC6_MC1	0x23
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC7_OVF	0x24
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC7_MC0	0x25
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TC7_MC1	0x26
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_ADC_RESRDY	0x27
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_DAC_EMPTY	0x28
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_I2S_RX_0	0x29
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_I2S_RX_1	0x2a
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_I2S_TX_0	0x2b
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_I2S_TX_1	0x2c
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC3_OVF	0x2d
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC3_MC0	0x2e
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC3_MC1	0x2f
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC3_MC2	0x30
#define  SAMD21_DMAC_CHCTRLB_TRIGSRC_TCC3_MC3	0x31

#define SAMD21_DMAC_CHCTRLB_TRIGACT	22
#define  SAMD21_DMAC_CHCTRLB_TRIGACT_BLOCK		0
#define  SAMD21_DMAC_CHCTRLB_TRIGACT_BEAT		2
#define  SAMD21_DMAC_CHCTRLB_TRIGACT_TRANSACTION	3

#define SAMD21_DMAC_CHCTRLB_CMD		24
#define  SAMD21_DMAC_CHCTRLB_CMD_NOACT		0
#define  SAMD21_DMAC_CHCTRLB_CMD_SUSPEND	1
#define  SAMD21_DMAC_CHCTRLB_CMD_RESUME		2

#define SAMD21_DMAC_CHINTFLAG_TERR	0
#define SAMD21_DMAC_CHINTFLAG_TCMPL	1
#define SAMD21_DMAC_CHINTFLAG_SUSP	2

#define SAMD21_DMAC_CHSTATUS_PEND	0
#define SAMD21_DMAC_CHSTATUS_BUSY	1
#define SAMD21_DMAC_CHSTATUS_FERR	2

#define SAMD21_DMAC_DESC_BTCTRL_VALID		0
#define SAMD21_DMAC_DESC_BTCTRL_EVOSEL		1
#define SAMD21_DMAC_DESC_BTCTRL_BLOCKACT	3
#define SAMD21_DMAC_DESC_BTCTRL_BEATSIZE	8
#define SAMD21_DMAC_DESC_BTCTRL_SRCINC		10
#define SAMD21_DMAC_DESC_BTCTRL_DSTINC		11
#define SAMD21_DMAC_DESC_BTCTRL_STEPSEL		12
#define SAMD21_DMAC_DESC_BTCTRL_STEPSIZE	13

struct samd21_nvmctrl {
	vuint32_t	ctrla;
	vuint32_t	ctrlb;
	vuint32_t	param;
	vuint32_t	intenclr;

	vuint32_t	intenset;
	vuint32_t	intflag;
	vuint32_t	status;
	vuint32_t	addr;

	vuint32_t	lock;
};

extern struct samd21_nvmctrl samd21_nvmctrl;

#define samd21_nvmctrl (*(struct samd21_nvmctrl *) 0x41004000)

#define SAMD21_NVMCTRL_CTRLA_CMD	0
#define  SAMD21_NVMCTRL_CTRLA_CMD_ER		0x02
#define  SAMD21_NVMCTRL_CTRLA_CMD_WP		0x04
#define  SAMD21_NVMCTRL_CTRLA_CMD_EAR		0x05
#define  SAMD21_NVMCTRL_CTRLA_CMD_WAP		0x06
#define  SAMD21_NVMCTRL_CTRLA_CMD_RWWEEER	0x1a
#define  SAMD21_NVMCTRL_CTRLA_CMD_RWEEEWP	0x1c
#define  SAMD21_NVMCTRL_CTRLA_CMD_LR		0x40
#define  SAMD21_NVMCTRL_CTRLA_CMD_UR		0x41
#define  SAMD21_NVMCTRL_CTRLA_CMD_SPRM		0x42
#define  SAMD21_NVMCTRL_CTRLA_CMD_CPRM		0x43
#define  SAMD21_NVMCTRL_CTRLA_CMD_PBC		0x44
#define  SAMD21_NVMCTRL_CTRLA_CMD_SSB		0x45
#define  SAMD21_NVMCTRL_CTRLA_CMD_INVALL	0x46
#define  SAMD21_NVMCTRL_CTRLA_CMD_LDR		0x47
#define  SAMD21_NVMCTRL_CTRLA_CMD_UDR		0x48
#define SAMD21_NVMCTRL_CTRLA_CMDEX	8
#define  SAMD21_NVMCTRL_CTRLA_CMDEX_KEY		0xa5

#define SAMD21_NVMCTRL_CTRLB_RWS	1
#define SAMD21_NVMCTRL_CTRLB_MANW	7
#define SAMD21_NVMCTRL_CTRLB_SLEEPRM	8
#define SAMD21_NVMCTRL_CTRLB_READMODE	16
#define SAMD21_NVMCTRL_CTRLB_CACHEDIS	18

#define SAMD21_NVMCTRL_INTENCLR_READY	0
#define SAMD21_NVMCTRL_INTENCLR_ERROR	1

#define SAMD21_NVMCTRL_INTENSET_READY	0
#define SAMD21_NVMCTRL_INTENSET_ERROR	1

#define SAMD21_NVMCTRL_INTFLAG_READY	0
#define SAMD21_NVMCTRL_INTFLAG_ERROR	1

#define SAMD21_NVMCTRL_STATUS_PRM	0
#define SAMD21_NVMCTRL_STATUS_LOAD	1
#define SAMD21_NVMCTRL_STATUS_PROGE	2
#define SAMD21_NVMCTRL_STATUS_LOCKE	3
#define SAMD21_NVMCTRL_STATUS_NVME	4
#define SAMD21_NVMCTRL_STATUS_SB	8

#define SAMD21_NVMCTRL_PARAM_NVMP	0
#define  SAMD21_NVMCTRL_PARAM_NVMP_MASK		0xffff
#define SAMD21_NVMCTRL_PARAM_PSZ	16
#define  SAMD21_NVMCTRL_PARAM_PSZ_MASK		0x7
#define SAMD21_NVMCTRL_PARAM_RWWEEP	20
#define  SAMD21_NVMCTRL_PARAM_RWWEEP_MASK	0xfff

static inline uint32_t
samd21_nvmctrl_page_size(void)
{
	return 1 << (3 + ((samd21_nvmctrl.param >> SAMD21_NVMCTRL_PARAM_PSZ) &
			  SAMD21_NVMCTRL_PARAM_PSZ_MASK));
}

uint32_t
samd21_flash_size(void);

struct samd21_port {
	vuint32_t	dir;
	vuint32_t	dirclr;
	vuint32_t	dirset;
	vuint32_t	dirtgl;

	vuint32_t	out;
	vuint32_t	outclr;
	vuint32_t	outset;
	vuint32_t	outtgl;

	vuint32_t	in;
	vuint32_t	ctrl;
	vuint32_t	wrconfig;
	vuint32_t	reserved_2c;

	vuint8_t	pmux[16];

	vuint8_t	pincfg[32];
};

extern struct samd21_port samd21_port_a;
extern struct samd21_port samd21_port_b;

#define samd21_port_a (*(struct samd21_port *) 0x41004400)
#define samd21_port_b (*(struct samd21_port *) 0x41004480)

#define SAMD21_PORT_PINCFG_PMUXEN	0
#define SAMD21_PORT_PINCFG_INEN		1
#define SAMD21_PORT_PINCFG_PULLEN	2
#define SAMD21_PORT_PINCFG_DRVSTR	6

#define SAMD21_PORT_PMUX_FUNC_A		0
#define SAMD21_PORT_PMUX_FUNC_B		1
#define SAMD21_PORT_PMUX_FUNC_C		2
#define SAMD21_PORT_PMUX_FUNC_D		3
#define SAMD21_PORT_PMUX_FUNC_E		4
#define SAMD21_PORT_PMUX_FUNC_F		5
#define SAMD21_PORT_PMUX_FUNC_G		6
#define SAMD21_PORT_PMUX_FUNC_H		7
#define SAMD21_PORT_PMUX_FUNC_I		8

#define SAMD21_PORT_DIR_OUT		1
#define SAMD21_PORT_DIR_IN		0

static inline void
samd21_port_dir_set(struct samd21_port *port, uint8_t pin, uint8_t dir)
{
	if (dir)
		port->dirset = (1 << pin);
	else
		port->dirclr = (1 << pin);
}

static inline void
samd21_port_pincfg_set(struct samd21_port *port, uint8_t pin, uint8_t pincfg_mask, uint8_t pincfg)
{
	port->pincfg[pin] = (port->pincfg[pin] & ~pincfg_mask) | pincfg;
}

static inline uint8_t
samd21_port_pincfg_get(struct samd21_port *port, uint8_t pin)
{
	return port->pincfg[pin];
}

static inline void
samd21_port_pmux_set(struct samd21_port *port, uint8_t pin, uint8_t func)
{
	uint8_t	byte = pin >> 1;
	uint8_t bit = (pin & 1) << 2;
	uint8_t mask = 0xf << bit;
	uint8_t value = (port->pmux[byte] & ~mask) | (func << bit);
	port->pmux[byte] = value;
	samd21_port_pincfg_set(port, pin,
			       (1 << SAMD21_PORT_PINCFG_PMUXEN),
			       (1 << SAMD21_PORT_PINCFG_PMUXEN));
}

struct samd21_adc {
	vuint8_t	ctrla;
	vuint8_t	refctrl;
	vuint8_t	avgctrl;
	vuint8_t	sampctrl;
	vuint16_t	ctrlb;
	vuint16_t	reserved_06;
	vuint8_t	winctrl;
	vuint8_t	reserved_09;
	vuint16_t	reserved_0a;
	vuint8_t	swtrig;
	vuint8_t	reserved_0d;
	vuint16_t	reserved_0e;

	vuint32_t	inputctrl;
	vuint8_t	evctrl;
	vuint8_t	reserved_15;
	vuint8_t	intenclr;
	vuint8_t	intenset;
	vuint8_t	intflag;
	vuint8_t	status;
	vuint16_t	result;
	vuint16_t	winlt;
	vuint16_t	reserved_1e;

	vuint16_t	winut;
	vuint16_t	reserved_22;
	vuint16_t	gaincorr;
	vuint16_t	offsetcorr;
	vuint16_t	calib;
	vuint8_t	dbgctrl;
	vuint8_t	reserved_2b;
	vuint32_t	reserved_2c;
};

#define SAMD21_ADC_CTRLA_SWRST		0
#define SAMD21_ADC_CTRLA_ENABLE		1
#define SAMD21_ADC_CTRLA_RUNSTDBY	2

#define SAMD21_ADC_REFCTRL_REFSEL	0
#define  SAMD21_ADC_REFCTRL_REFSEL_INT1V	0
#define  SAMD21_ADC_REFCTRL_REFSEL_INTVCC0	1
#define  SAMD21_ADC_REFCTRL_REFSEL_INTVCC1	2
#define  SAMD21_ADC_REFCTRL_REFSEL_VREFA	3
#define  SAMD21_ADC_REFCTRL_REFSEL_VREFB	4
#define SAMD21_ADC_REFCTRL_REFCOMP	7

#define SAMD21_ADC_AVGCTRL_SAMPLENUM	0
#define SAMD21_ADC_AVGCTRL_ADJRES	4

#define SAMD21_ADC_SAMPCTRL_SAMPLEN	0

#define SAMD21_ADC_CTRLB_DIFFMODE	0
#define SAMD21_ADC_CTRLB_LEFTADJ	1
#define SAMD21_ADC_CTRLB_FREERUN	2
#define SAMD21_ADC_CTRLB_CORREN		3
#define SAMD21_ADC_CTRLB_RESSEL		4
#define  SAMD21_ADC_CTRLB_RESSEL_12BIT		0
#define  SAMD21_ADC_CTRLB_RESSEL_16BIT		1
#define  SAMD21_ADC_CTRLB_RESSEL_10BIT		2
#define  SAMD21_ADC_CTRLB_RESSEL_8BIT		3
#define SAMD21_ADC_CTRLB_PRESCALER	8
#define  SAMD21_ADC_CTRLB_PRESCALER_DIV4	0
#define  SAMD21_ADC_CTRLB_PRESCALER_DIV8	1
#define  SAMD21_ADC_CTRLB_PRESCALER_DIV16	2
#define  SAMD21_ADC_CTRLB_PRESCALER_DIV32	3
#define  SAMD21_ADC_CTRLB_PRESCALER_DIV64	4
#define  SAMD21_ADC_CTRLB_PRESCALER_DIV128	5
#define  SAMD21_ADC_CTRLB_PRESCALER_DIV256	6
#define  SAMD21_ADC_CTRLB_PRESCALER_DIV512	7

#define SAMD21_ADC_SWTRIG_FLUSH		0
#define SAMD21_ADC_SWTRIG_START		1

#define SAMD21_ADC_INPUTCTRL_MUXPOS		0
# define SAMD21_ADC_INPUTCTRL_MUXPOS_BANDGAP		0x19
# define SAMD21_ADC_INPUTCTRL_MUXPOS_SCALEDCOREVCC	0x1a
# define SAMD21_ADC_INPUTCTRL_MUXPOS_SCALEDIOVCC	0x1b
# define SAMD21_ADC_INPUTCTRL_MUXPOS_DAC		0x1c
#define SAMD21_ADC_INPUTCTRL_MUXNEG		8
# define SAMD21_ADC_INPUTCTRL_MUXNEG_GND		0x18
# define SAMD21_ADC_INPUTCTRL_MUXNEG_IOGND		0x19
#define SAMD21_ADC_INPUTCTRL_INPUTSCAN		16
#define SAMD21_ADC_INPUTCTRL_INPUTOFFSET	20
#define SAMD21_ADC_INPUTCTRL_GAIN		24
#define  SAMD21_ADC_INPUTCTRL_GAIN_1X			0
#define  SAMD21_ADC_INPUTCTRL_GAIN_DIV2			0xf

#define SAMD21_ADC_INTFLAG_RESRDY	0
#define SAMD21_ADC_INTFLAG_OVERRUN	1
#define SAMD21_ADC_INTFLAG_WINMON	2
#define SAMD21_ADC_INTFLAG_SYNCRDY	3

#define SAMD21_ADC_STATUS_SYNCBUSY	7

#define SAMD21_ADC_CALIB_LINEARITY_CAL	0
#define SAMD21_ADC_CALIB_BIAS_CAL	16

extern struct samd21_adc samd21_adc;

#define samd21_adc (*(struct samd21_adc *) 0x42004000)

/* TC */
struct samd21_tc {
	vuint16_t	ctrla;
	vuint16_t	readreq;
	vuint8_t	ctrlbclr;
	vuint8_t	ctrlbset;
	vuint8_t	ctrlc;
	vuint8_t	reserved_07;
	vuint8_t	dbgctrl;
	vuint8_t	reserved_09;
	vuint16_t	evctrl;
	vuint8_t	intenclr;
	vuint8_t	intenset;
	vuint8_t	intflag;
	vuint8_t	status;

	union {
		struct {
			vuint8_t	count;
			vuint8_t	reserved_11;
			vuint16_t	reserved_12;
			vuint8_t	per;
			vuint8_t	reserved_15;
			vuint16_t	reserved_16;
			vuint8_t	cc[2];
		} mode_8;
		struct {
			vuint16_t	count;
			vuint16_t	reserved_12;
			vuint32_t	reserved_14;
			vuint16_t	cc[2];
		} mode_16;
		struct {
			vuint32_t	count;
			vuint32_t	reserved_14;
			vuint32_t	cc[2];
		} mode_32;
	};
};

extern struct samd21_tc samd21_tc3;
#define samd21_tc3 (*(struct samd21_tc *) 0x42002c00)

extern struct samd21_tc samd21_tc4;
#define samd21_tc4 (*(struct samd21_tc *) 0x42003000)

extern struct samd21_tc samd21_tc5;
#define samd21_tc5 (*(struct samd21_tc *) 0x42003400)

extern struct samd21_tc samd21_tc6;
#define samd21_tc6 (*(struct samd21_tc *) 0x42003800)

extern struct samd21_tc samd21_tc7;
#define samd21_tc7 (*(struct samd21_tc *) 0x42003c00)

#define SAMD21_TC_CTRLA_SWRST		0
#define SAMD21_TC_CTRLA_ENABLE		1
#define SAMD21_TC_CTRLA_MODE		2
#define  SAMD21_TC_CTRLA_MODE_COUNT16		0
#define  SAMD21_TC_CTRLA_MODE_COUNT8		1
#define  SAMD21_TC_CTRLA_MODE_COUNT32		2
#define SAMD21_TC_CTRLA_WAVEGEN		5
#define  SAMD21_TC_CTRLA_WAVEGEN_NFRQ		0
#define  SAMD21_TC_CTRLA_WAVEGEN_MFRQ		1
#define  SAMD21_TC_CTRLA_WAVEGEN_NPWM		2
#define  SAMD21_TC_CTRLA_WAVEGEN_MPWM		3
#define SAMD21_TC_CTRLA_PRESCALER	8
#define  SAMD21_TC_CTRLA_PRESCALER_DIV1		0
#define  SAMD21_TC_CTRLA_PRESCALER_DIV2		1
#define  SAMD21_TC_CTRLA_PRESCALER_DIV4		2
#define  SAMD21_TC_CTRLA_PRESCALER_DIV8		3
#define  SAMD21_TC_CTRLA_PRESCALER_DIV16	4
#define  SAMD21_TC_CTRLA_PRESCALER_DIV64	5
#define  SAMD21_TC_CTRLA_PRESCALER_DIV256	6
#define  SAMD21_TC_CTRLA_PRESCALER_DIV1024	7
#define SAMD21_TC_CTRLA_RUNSTDBY	11
#define SAMD21_TC_CTRLA_PRESCSYNC	12
#define  SAMD21_TC_CTRLA_PRESCSYNC_GCLK		0
#define  SAMD21_TC_CTRLA_PRESCSYNC_PRSEC	1
#define  SAMD21_TC_CTRLA_PRESCSYNC_RESYNC	2

#define SAMD21_TC_READREQ_ADDR		0
#define SAMD21_TC_READREQ_RCONT		14
#define SAMD21_TC_READREQ_RREQ		15
#define SAMD21_TC_CTRLB_DIR		0
#define SAMD21_TC_CTRLB_ONESHOT		2
#define SAMD21_TC_CTRLB_CMD		6
#define SAMD21_TC_CTRLC_INVEN(x)	(0 + (x))
#define SAMD21_TC_CTRLC_CPTEN(x)	(4 + (x))
#define SAMD21_TC_DBGCTRL_DBGRUN	0
#define SAMD21_TC_EVCTRL_EVACT		0
#define SAMD21_TC_EVCTRL_TCINV		4
#define SAMD21_TC_EVCTRL_TCEI		5
#define SAMD21_TC_EVCTRL_OVFEO		8
#define SAMD21_TC_EVCTRL_MCEO(x)	(12 + (x))

#define SAMD21_TC_INTFLAG_MC(x)		(4 + (x))
#define SAMD21_TC_INTFLAG_SYNCRDY	3
#define SAMD21_TC_INTFLAG_ERR		1
#define SAMD21_TC_INTFLAG_OVF		0

#define SAMD21_TC_STATUS_STOP		3
#define SAMD21_TC_STATUS_SLAVE		4
#define SAMD21_TC_STATUS_SYNCBUSY	7

/* TCC */

struct samd21_tcc {
	vuint32_t	ctrla;
	vuint8_t	ctrlbclr;
	vuint8_t	ctrlbset;
	vuint16_t	reserved_06;
	vuint32_t	syncbusy;
	vuint32_t	fctrla;

	vuint32_t	fctlrb;
	vuint32_t	wexctrl;
	vuint32_t	drvctrl;
	vuint16_t	reserved_1c;
	vuint8_t	dbgctrl;
	vuint8_t	reserved_1f;

	vuint32_t	evctrl;
	vuint32_t	intenclr;
	vuint32_t	intenset;
	vuint32_t	intflag;

	vuint32_t	status;
	vuint32_t	count;
	vuint16_t	patt;
	vuint16_t	reserved_3a;
	vuint32_t	wave;

	vuint32_t	per;
	vuint32_t	cc[4];
	vuint32_t	reserved_54;
	vuint32_t	reserved_58;
	vuint32_t	reserved_5c;

	vuint32_t	reserved_60;
	vuint16_t	pattb;
	vuint16_t	reserved_66;
	vuint32_t	waveb;
	vuint32_t	perb;

	vuint32_t	ccb[4];
};

extern struct samd21_tcc samd21_tcc0;
#define samd21_tcc0 (*(struct samd21_tcc *) 0x42002000)

extern struct samd21_tcc samd21_tcc1;
#define samd21_tcc1 (*(struct samd21_tcc *) 0x42002400)

extern struct samd21_tcc samd21_tcc2;
#define samd21_tcc2 (*(struct samd21_tcc *) 0x42002800)

extern struct samd21_tcc samd21_tcc3;
#define samd21_tcc3 (*(struct samd21_tcc *) 0x42006000)

#define SAMD21_TCC_CTRLA_SWRST		0
#define SAMD21_TCC_CTRLA_ENABLE		1
#define SAMD21_TCC_CTRLA_RESOLUTION	5
#define  SAMD21_TCC_CTRLA_RESOLUTION_NONE	0
#define  SAMD21_TCC_CTRLA_RESOLUTION_DITH4	1
#define  SAMD21_TCC_CTRLA_RESOLUTION_DITH5	2
#define  SAMD21_TCC_CTRLA_RESOLUTION_DITH6	3
#define SAMD21_TCC_CTRLA_PRESCALER	8
#define  SAMD21_TCC_CTRLA_PRESCALER_DIV1	0
#define  SAMD21_TCC_CTRLA_PRESCALER_DIV2	1
#define  SAMD21_TCC_CTRLA_PRESCALER_DIV4	2
#define  SAMD21_TCC_CTRLA_PRESCALER_DIV8	3
#define  SAMD21_TCC_CTRLA_PRESCALER_DIV16	4
#define  SAMD21_TCC_CTRLA_PRESCALER_DIV64	5
#define  SAMD21_TCC_CTRLA_PRESCALER_DIV256	6
#define  SAMD21_TCC_CTRLA_PRESCALER_DIV1024	7
#define SAMD21_TCC_CTRLA_RUNSTDBY	11
#define SAMD21_TCC_CTRLA_PRESYNC	12
#define  SAMD21_TCC_CTRLA_PRESYNC_GCLK		0
#define  SAMD21_TCC_CTRLA_PRESYNC_PRESC		1
#define  SAMD21_TCC_CTRLA_PRESYNC_RESYNC	2
#define SAMD21_TCC_CTRLA_ALOCK		14
#define SAMD21_TCC_CTRLA_CPTEN(n)	(24 + (n))

#define SAMD21_TCC_CTRLB_DIR		0
#define SAMD21_TCC_CTRLB_LUPD		1
#define SAMD21_TCC_CTRLB_ONESHOT	2
#define SAMD21_TCC_CTRLB_IDXCMD		3
#define  SAMD21_TCC_CTRLB_IDXCMD_DISABLE	0
#define  SAMD21_TCC_CTRLB_IDXCMD_SET		1
#define  SAMD21_TCC_CTRLB_IDXCMD_CLEAR		2
#define  SAMD21_TCC_CTRLB_IDXCMD_HOLD		3
#define SAMD21_TCC_CTRLB_CMD		5
#define  SAMD21_TCC_CTRLB_CMD_NONE		0
#define  SAMD21_TCC_CTRLB_CMD_RETRIGGER		1
#define  SAMD21_TCC_CTRLB_CMD_STOP		2
#define  SAMD21_TCC_CTRLB_CMD_UPDATE		3
#define  SAMD21_TCC_CTRLB_CMD_READSYNC		4
#define  SAMD21_TCC_CTRLB_CMD_DMAOS		5

#define SAMD21_TCC_SYNCBUSY_SWRST	0
#define SAMD21_TCC_SYNCBUSY_ENABLE	1
#define SAMD21_TCC_SYNCBUSY_CTRLB	2
#define SAMD21_TCC_SYNCBUSY_STATUS	3
#define SAMD21_TCC_SYNCBUSY_COUNT	4
#define SAMD21_TCC_SYNCBUSY_PATT	5
#define SAMD21_TCC_SYNCBUSY_WAVE	6
#define SAMD21_TCC_SYNCBUSY_PER		7
#define SAMD21_TCC_SYNCBUSY_CC(x)	(8 + (x))
#define SAMD21_TCC_SYNCBUSY_PATTB	16
#define SAMD21_TCC_SYNCBUSY_WAVEB	17
#define SAMD21_TCC_SYNCBUSY_PERB	18
#define SAMD21_TCC_SYNCBUSY_CCB(x)	((19 + (x))

#define SAMD21_TCC_EVCTRL_EVACTO	0
#define SAMD21_TCC_EVCTRL_EVACT1	3
#define SAMD21_TCC_EVCTRL_CNTSEL	6
#define SAMD21_TCC_EVCTRL_OVFEO		8
#define SAMD21_TCC_EVCTRL_TRGEO		9
#define SAMD21_TCC_EVCTRL_CNTEO		10
#define SAMD21_TCC_EVCTRL_TCINV(x)	(12 + (x))
#define SAMD21_TCC_EVCTRL_MCEI(x)	(16 + (x))
#define SAMD21_TCC_EVCTRL_MCEO(x)	(24 + (x))

#define SAMD21_TCC_INTFLAG_OVF		0
#define SAMD21_TCC_INTFLAG_TRG		1
#define SAMD21_TCC_INTFLAG_CNT		2
#define SAMD21_TCC_INTFLAG_ERR		3
#define SAMD21_TCC_INTFLAG_UFS		10
#define SAMD21_TCC_INTFLAG_DFS		11
#define SAMD21_TCC_INTFLAG_FAULTA	12
#define SAMD21_TCC_INTFLAG_FAULTB	13
#define SAMD21_TCC_INTFLAG_FAULT0	14
#define SAMD21_TCC_INTFLAG_FAULT1	15
#define SAMD21_TCC_INTFLAG_MC(x)	(16 + (x))

#define SAMD21_TCC_WAVE_WAVEGEN		0
#define  SAMD21_TCC_WAVE_WAVEGEN_NFRQ		0
#define  SAMD21_TCC_WAVE_WAVEGEN_MFRQ		1
#define  SAMD21_TCC_WAVE_WAVEGEN_NPWM		2
#define  SAMD21_TCC_WAVE_WAVEGEN_DSCRITICAL	4
#define  SAMD21_TCC_WAVE_WAVEGEN_DSBOTTOM	5
#define  SAMD21_TCC_WAVE_WAVEGEN_DSBOTH		6
#define  SAMD21_TCC_WAVE_WAVEGEN_DSTOP		7
#define SAMD21_TCC_WAVE_RAMP		4
#define SAMD21_TCC_WAVE_CIPEREN		7
#define SAMD21_TCC_WAVE_CCCEN(x)	(8 + (x))
#define SAMD21_TCC_WAVE_POL(x)		(16 + (x))
#define SAMD21_TCC_WAVE_SWAP(x)		(24 + (x))

/* USB */

struct samd21_usb {
	vuint8_t	ctrla;
	vuint8_t	reserved_01;
	vuint8_t	syncbusy;
	vuint8_t	qosctrl;

	vuint32_t	reserved_04;
	vuint16_t	ctrlb;
	vuint8_t	dadd;
	vuint8_t	reserved_0b;
	vuint8_t	status;
	vuint8_t	fsmstatus;
	vuint16_t	reserved_0e;

	vuint16_t	fnum;
	vuint16_t	reserved_12;
	vuint16_t	intenclr;
	vuint16_t	reserved_16;
	vuint16_t	intenset;
	vuint16_t	reserved_1a;
	vuint16_t	intflag;
	vuint16_t	reserved_1e;

	vuint16_t	epintsmry;
	vuint16_t	reserved_22;

	vuint32_t	descadd;
	vuint16_t	padcal;
	uint8_t		reserved_2a[0x100 - 0x2a];

	struct {
		vuint8_t	epcfg;
		vuint8_t	reserved_01;
		vuint8_t	reserved_02;
		vuint8_t	binterval;
		vuint8_t	epstatusclr;
		vuint8_t	epstatusset;
		vuint8_t	epstatus;
		vuint8_t	epintflag;
		vuint8_t	epintenclr;
		vuint8_t	epintenset;
		vuint8_t	reserved_0a[0x20 - 0x0a];
	} ep[8];
};

extern struct samd21_usb samd21_usb;

#define samd21_usb (*(struct samd21_usb *) 0x41005000)

#define SAMD21_USB_CTRLA_SWRST		0
#define SAMD21_USB_CTRLA_ENABLE		1
#define SAMD21_USB_CTRLA_RUNSTDBY	2
#define SAMD21_USB_CTRLA_MODE		7

#define SAMD21_USB_SYNCBUSY_SWRST	0
#define SAMD21_USB_SYNCBUSY_ENABLE	1

#define SAMD21_USB_QOSCTRL_CQOS		0
#define SAMD21_USB_QOSCTRL_DQOS		2

#define SAMD21_USB_CTRLB_DETACH		0
#define SAMD21_USB_CTRLB_UPRSM		1
#define SAMD21_USB_CTRLB_SPDCONF	2
#define  SAMD21_USB_CTRLB_SPDCONF_FS		0
#define  SAMD21_USB_CTRLB_SPDCONF_LS		1
#define  SAMD21_USB_CTRLB_SPDCONF_MASK		0x3
#define SAMD21_USB_CTRLB_NREPLY		4
#define SAMD21_USB_CTRLB_GNAK		9
#define SAMD21_USB_CTRLB_LPMHDSK	10
#define  SAMD21_USB_CTRLB_LPMHDSK_NONE		0
#define  SAMD21_USB_CTRLB_LPMHDSK_ACK		1
#define  SAMD21_USB_CTRLB_LPMHDSK_NYET		2
#define  SAMD21_USB_CTRLB_LPMHDSK_MASK		3

#define SAMD21_USB_DADD_DADD		0
#define SAMD21_USB_DADD_ADDEN		7

#define SAMD21_USB_STATUS_SPEED		2
#define SAMD21_USB_STATUS_LINESTATE	6
#define SAMD21_USB_FNUM_MFNUM		0
#define SAMD21_USB_FNUM_FNUM		3
#define SAMD21_USB_FNUM_FNCERR		15
#define SAMD21_USB_INTFLAG_SUSPEND	0
#define SAMD21_USB_INTFLAG_SOF		2
#define SAMD21_USB_INTFLAG_EORST	3
#define SAMD21_USB_INTFLAG_WAKEUP	4
#define SAMD21_USB_INTFLAG_EORSM	5
#define SAMD21_USB_INTFLAG_UPRSM	6
#define SAMD21_USB_INTFLAG_RAMACER	7
#define SAMD21_USB_INTFLAG_LPMNYET	8
#define SAMD21_USB_INTFLAG_LPMSUSP	9

#define SAMD21_USB_PADCAL_TRANSP	0
#define SAMD21_USB_PADCAL_TRANSN	6
#define SAMD21_USB_PADCAL_TRIM		12

#define SAMD21_USB_EP_EPCFG_EP_TYPE_OUT		0
#define  SAMD21_USB_EP_EPCFG_EP_TYPE_OUT_DISABLED	0
#define  SAMD21_USB_EP_EPCFG_EP_TYPE_OUT_CONTROL	1
#define  SAMD21_USB_EP_EPCFG_EP_TYPE_OUT_ISOCHRONOUS	2
#define  SAMD21_USB_EP_EPCFG_EP_TYPE_OUT_BULK		3
#define  SAMD21_USB_EP_EPCFG_EP_TYPE_OUT_INTERRUPT	4
#define  SAMD21_USB_EP_EPCFG_EP_TYPE_OUT_DUAL_BANK	5
#define SAMD21_USB_EP_EPCFG_EP_TYPE_IN		4
#define  SAMD21_USB_EP_EPCFG_EP_TYPE_IN_DISABLED	0
#define  SAMD21_USB_EP_EPCFG_EP_TYPE_IN_CONTROL	1
#define  SAMD21_USB_EP_EPCFG_EP_TYPE_IN_ISOCHRONOUS	2
#define  SAMD21_USB_EP_EPCFG_EP_TYPE_IN_BULK		3
#define  SAMD21_USB_EP_EPCFG_EP_TYPE_IN_INTERRUPT	4
#define  SAMD21_USB_EP_EPCFG_EP_TYPE_IN_DUAL_BANK	5

#define SAMD21_USB_EP_EPSTATUS_DTGLOUT			0
#define SAMD21_USB_EP_EPSTATUS_DTGLIN			1
#define SAMD21_USB_EP_EPSTATUS_CURBK			2
#define SAMD21_USB_EP_EPSTATUS_STALLRQ0			4
#define SAMD21_USB_EP_EPSTATUS_STALLRQ1			5
#define SAMD21_USB_EP_EPSTATUS_BK0RDY			6
#define SAMD21_USB_EP_EPSTATUS_BK1RDY			7

#define SAMD21_USB_EP_EPINTFLAG_TRCPT0			0
#define SAMD21_USB_EP_EPINTFLAG_TRCPT1			1
#define SAMD21_USB_EP_EPINTFLAG_TRFAIL0			2
#define SAMD21_USB_EP_EPINTFLAG_TRFAIL1			3
#define SAMD21_USB_EP_EPINTFLAG_RXSTP			4
#define SAMD21_USB_EP_EPINTFLAG_STALL			5

struct samd21_usb_desc_bank {
	vuint32_t	addr;
	vuint32_t	pcksize;
	vuint16_t	extreg;
	vuint8_t	status_bk;
	vuint8_t	reserved_0b;
	vuint32_t	reserved_0c;
};

struct samd21_usb_desc {
	struct samd21_usb_desc_bank bank[2];
};

extern struct samd21_usb_desc	samd21_usb_desc[8];

#define SAMD21_USB_DESC_PCKSIZE_BYTE_COUNT		0
#define  SAMD21_USB_DESC_PCKSIZE_BYTE_COUNT_MASK		0x3fff
#define SAMD21_USB_DESC_PCKSIZE_MULTI_PACKET_SIZE	14
#define  SAMD21_USB_DESC_PCKSIZE_MULTI_PACKET_SIZE_MASK		0x3fff
#define SAMD21_USB_DESC_PCKSIZE_SIZE			28
#define  SAMD21_USB_DESC_PCKSIZE_SIZE_8				0
#define  SAMD21_USB_DESC_PCKSIZE_SIZE_16			1
#define  SAMD21_USB_DESC_PCKSIZE_SIZE_32			2
#define  SAMD21_USB_DESC_PCKSIZE_SIZE_64			3
#define  SAMD21_USB_DESC_PCKSIZE_SIZE_128			4
#define  SAMD21_USB_DESC_PCKSIZE_SIZE_256			5
#define  SAMD21_USB_DESC_PCKSIZE_SIZE_512			6
#define  SAMD21_USB_DESC_PCKSIZE_SIZE_1023			7
#define  SAMD21_USB_DESC_PCKSIZE_SIZE_MASK			7
#define SAMD21_USB_DESC_PCKSIZE_AUTO_ZLP		31

static inline uint32_t
samd21_usb_desc_get_byte_count(uint8_t ep, uint8_t bank)
{
	return ((samd21_usb_desc[ep].bank[bank].pcksize >> SAMD21_USB_DESC_PCKSIZE_BYTE_COUNT) &
		SAMD21_USB_DESC_PCKSIZE_BYTE_COUNT_MASK);
}

static inline void
samd21_usb_desc_set_byte_count(uint8_t ep, uint8_t bank, uint32_t count)
{
	uint32_t pcksize = samd21_usb_desc[ep].bank[bank].pcksize;

	pcksize &= ~(SAMD21_USB_DESC_PCKSIZE_BYTE_COUNT_MASK << SAMD21_USB_DESC_PCKSIZE_BYTE_COUNT);
	pcksize &= ~(SAMD21_USB_DESC_PCKSIZE_MULTI_PACKET_SIZE_MASK << SAMD21_USB_DESC_PCKSIZE_MULTI_PACKET_SIZE);
	pcksize |= (count << SAMD21_USB_DESC_PCKSIZE_BYTE_COUNT);
	samd21_usb_desc[ep].bank[bank].pcksize = pcksize;
}

static inline void
samd21_usb_desc_set_size(uint8_t ep, uint8_t bank, uint32_t size)
{
	uint32_t pcksize = samd21_usb_desc[ep].bank[bank].pcksize;

	pcksize &= ~(SAMD21_USB_DESC_PCKSIZE_SIZE_MASK << SAMD21_USB_DESC_PCKSIZE_SIZE);

	uint32_t size_bits = 0;
	switch (size) {
	case 8: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_8; break;
	case 16: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_16; break;
	case 32: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_32; break;
	case 64: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_64; break;
	case 128: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_128; break;
	case 256: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_256; break;
	case 512: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_512; break;
	case 1023: size_bits = SAMD21_USB_DESC_PCKSIZE_SIZE_1023; break;
	}
	pcksize |= (size_bits << SAMD21_USB_DESC_PCKSIZE_SIZE);
	samd21_usb_desc[ep].bank[bank].pcksize = pcksize;
}

static inline void
samd21_usb_ep_set_ready(uint8_t ep, uint8_t bank)
{
	samd21_usb.ep[ep].epstatusset = (1 << (SAMD21_USB_EP_EPSTATUS_BK0RDY + bank));
	samd21_usb.ep[ep].epintflag = (1 << (SAMD21_USB_EP_EPINTFLAG_TRFAIL0 + bank));
}

static inline void
samd21_usb_ep_clr_ready(uint8_t ep, uint8_t bank)
{
	samd21_usb.ep[ep].epstatusclr = (1 << (SAMD21_USB_EP_EPSTATUS_BK0RDY + bank));
}

static inline uint8_t
samd21_usb_ep_ready(uint8_t ep)
{
	return (samd21_usb.ep[ep].epstatus >> SAMD21_USB_EP_EPSTATUS_BK0RDY) & 3;
}

static inline uint8_t
samd21_usb_ep_curbk(uint8_t ep)
{
	return (samd21_usb.ep[ep].epstatus >> SAMD21_USB_EP_EPSTATUS_CURBK) & 1;
}

/* sercom */

struct samd21_sercom {
	vuint32_t	ctrla;
	vuint32_t	ctrlb;
	vuint32_t	reserved_08;
	vuint16_t	baud;
	vuint8_t	rxpl;
	vuint8_t	reserved_0f;

	vuint32_t	reserved_10;
	vuint8_t	intenclr;
	vuint8_t	reserved_15;
	vuint8_t	intenset;
	vuint8_t	reserved_17;
	vuint8_t	intflag;
	vuint8_t	reserved_19;
	vuint16_t	status;
	vuint32_t	syncbusy;

	vuint32_t	reserved_20;
	vuint32_t	addr;
	vuint16_t	data;
	vuint16_t	reserved_2a;
	vuint32_t	reserved_2c;

	vuint8_t	dbgctrl;
	vuint8_t	reserved_31;
	vuint16_t	reserved_32;
	vuint16_t	fifospace;
	vuint16_t	fifoptr;
};

extern struct samd21_sercom samd21_sercom0;
extern struct samd21_sercom samd21_sercom1;
extern struct samd21_sercom samd21_sercom2;
extern struct samd21_sercom samd21_sercom3;
extern struct samd21_sercom samd21_sercom4;
extern struct samd21_sercom samd21_sercom5;

#define samd21_sercom0	(*(struct samd21_sercom *) 0x42000800)
#define samd21_sercom1	(*(struct samd21_sercom *) 0x42000c00)
#define samd21_sercom2	(*(struct samd21_sercom *) 0x42001000)
#define samd21_sercom3	(*(struct samd21_sercom *) 0x42001400)
#define samd21_sercom4	(*(struct samd21_sercom *) 0x42001800)
#define samd21_sercom5	(*(struct samd21_sercom *) 0x42001c00)

#define SAMD21_SERCOM_CTRLA_SWRST	0
#define SAMD21_SERCOM_CTRLA_ENABLE	1
#define SAMD21_SERCOM_CTRLA_MODE	2
# define SAMD21_SERCOM_CTRLA_MODE_USART		1
# define SAMD21_SERCOM_CTRLA_MODE_I2C_MASTER	5

#define SAMD21_SERCOM_CTRLA_RUNSTDBY	7

/* USART mode */
#define SAMD21_SERCOM_CTRLA_IBON	8
#define SAMD21_SERCOM_CTRLA_SAMPR	13
#define SAMD21_SERCOM_CTRLA_TXPO	16
#define SAMD21_SERCOM_CTRLA_RXPO	20
#define SAMD21_SERCOM_CTRLA_SAMPA	22
#define SAMD21_SERCOM_CTRLA_FORM	24
#define SAMD21_SERCOM_CTRLA_CMODE	28
#define SAMD21_SERCOM_CTRLA_CPOL	29
#define SAMD21_SERCOM_CTRLA_DORD	30

/* I2C master mode */
#define SAMD21_SERCOM_CTRLA_PINOUT	16
#define SAMD21_SERCOM_CTRLA_SDAHOLD	20
#define  SAMD21_SERCOM_CTRLA_SDAHOLD_DIS	0
#define  SAMD21_SERCOM_CTRLA_SDAHOLD_75NS	1
#define  SAMD21_SERCOM_CTRLA_SDAHOLD_450NS	2
#define  SAMD21_SERCOM_CTRLA_SDAHOLD_600NS	3
#define SAMD21_SERCOM_CTRLA_MEXTTOEN	22
#define SAMD21_SERCOM_CTRLA_SEXTTOEN	23
#define SAMD21_SERCOM_CTRLA_SPEED	24
#define  SAMD21_SERCOM_CTRLA_SPEED_STANDARD	0
#define  SAMD21_SERCOM_CTRLA_SPEED_FAST		1
#define  SAMD21_SERCOM_CTRLA_SPEED_HIGH		2
#define SAMD21_SERCOM_CTRLA_SCLSM	27
#define SAMD21_SERCOM_CTRLA_INACTOUT	28
#define  SAMD21_SERCOM_CTRLA_INACTOUT_DIS	0
#define  SAMD21_SERCOM_CTRLA_INACTOUT_55US	1
#define  SAMD21_SERCOM_CTRLA_INACTOUT_105US	2
#define  SAMD21_SERCOM_CTRLA_INACTOUT_205US	3
#define SAMD21_SERCOM_CTRLA_LOWTOUT	30

/* USART mode */
#define SAMD21_SERCOM_CTRLB_CHSIZE	0
#define SAMD21_SERCOM_CTRLB_SBMODE	6
#define SAMD21_SERCOM_CTRLB_COLDEN	8
#define SAMD21_SERCOM_CTRLB_SFDE	9
#define SAMD21_SERCOM_CTRLB_ENC		10
#define SAMD21_SERCOM_CTRLB_PMODE	13
#define SAMD21_SERCOM_CTRLB_TXEN	16
#define SAMD21_SERCOM_CTRLB_RXEN	17
#define SAMD21_SERCOM_CTRLB_FIFOCLR	22

/* I2C mode */
#define SAMD21_SERCOM_CTRLB_SMEN	8
#define SAMD21_SERCOM_CTRLB_QCEN	9
#define SAMD21_SERCOM_CTRLB_CMD		16
#define  SAMD21_SERCOM_CTRLB_CMD_NOP		0
#define  SAMD21_SERCOM_CTRLB_CMD_START		1
#define  SAMD21_SERCOM_CTRLB_CMD_READ		2
#define  SAMD21_SERCOM_CTRLB_CMD_STOP		3
#define SAMD21_SERCOM_CTRLB_ACKACT	18
#define  SAMD21_SERCOM_CTRLB_ACKACT_ACK		0
#define  SAMD21_SERCOM_CTRLB_ACKACT_NACK	1
#define SAMD21_SERCOM_CTRLB_FIFOCLR	22

/* USART mode */
#define SAMD21_SERCOM_INTFLAG_DRE	0
#define SAMD21_SERCOM_INTFLAG_TXC	1
#define SAMD21_SERCOM_INTFLAG_RXC	2
#define SAMD21_SERCOM_INTFLAG_RXS	3
#define SAMD21_SERCOM_INTFLAG_CTSIC	4
#define SAMD21_SERCOM_INTFLAG_RXBRK	5
#define SAMD21_SERCOM_INTFLAG_ERROR	7

/* I2C mode */
#define SAMD21_SERCOM_INTFLAG_ERROR	7
#define SAMD21_SERCOM_INTFLAG_RXFF	4
#define SAMD21_SERCOM_INTFLAG_TXFE	3
#define SAMD21_SERCOM_INTFLAG_SB	1
#define SAMD21_SERCOM_INTFLAG_MB	0

#define SAMD21_SERCOM_INTENCLR_DRE	0
#define SAMD21_SERCOM_INTENCLR_TXC	1
#define SAMD21_SERCOM_INTENCLR_RXC	2
#define SAMD21_SERCOM_INTENCLR_RXS	3
#define SAMD21_SERCOM_INTENCLR_CTSIC	4
#define SAMD21_SERCOM_INTENCLR_RXBRK	5
#define SAMD21_SERCOM_INTENCLR_ERROR	7

#define SAMD21_SERCOM_STATUS_PERR	0
#define SAMD21_SERCOM_STATUS_FERR	1
#define SAMD21_SERCOM_STATUS_BUFOVF	2
#define SAMD21_SERCOM_STATUS_CTS		3
#define SAMD21_SERCOM_STATUS_ISF		4
#define SAMD21_SERCOM_STATUS_COLL	5
#define SAMD21_SERCOM_STATUS_TXE		6

#define SAMD21_SERCOM_SYNCBUSY_SWRST	0
#define SAMD21_SERCOM_SYNCBUSY_ENABLE	1
#define SAMD21_SERCOM_SYNCBUSY_CTRLB	2
#define SAMD21_SERCOM_SYNCBUSY_SYSOP	2

#define SAMD21_SERCOM_ADDR_ADDR		0
#define SAMD21_SERCOM_ADDR_LENEN	13
#define SAMD21_SERCOM_ADDR_HS		14
#define SAMD21_SERCOM_ADDR_TENBITEN	15
#define SAMD21_SERCOM_ADDR_LEN		16

#define SAMD21_SERCOM_DBGCTRL_DBGSTOP	0

#define SAMD21_SERCOM_FIFOSPACE_TXSPACE	0
#define  SAMD21_SERCOM_FIFOSPACE_TXSPACE_MASK	0x1f
#define SAMD21_SERCOM_FIFOSPACE_RXSPACE	8
#define  SAMD21_SERCOM_FIFOSPACE_RXSPACE_MASK	0x1f

#define SAMD21_SERCOM_FIFOPTR_CPUWRPTR	0
#define  SAMD21_SERCOM_FIFOPTR_CPUWRPTR_MASK	0xf
#define SAMD21_SERCOM_FIFOPTR_CPURDPTR	8
#define  SAMD21_SERCOM_FIFOPTR_CPURDPTR_MASK	0xf

/* The SYSTICK starts at 0xe000e010 */
struct samd21_systick {
	vuint32_t	csr;
	vuint32_t	rvr;
	vuint32_t	cvr;
	vuint32_t	calib;
};

extern struct samd21_systick samd21_systick;

#define samd21_systick (*(struct samd21_systick *) 0xe000e010)

#define SAMD21_SYSTICK_CSR_ENABLE	0
#define SAMD21_SYSTICK_CSR_TICKINT	1
#define SAMD21_SYSTICK_CSR_CLKSOURCE	2
#define  SAMD21_SYSTICK_CSR_CLKSOURCE_EXTERNAL		0
#define  SAMD21_SYSTICK_CSR_CLKSOURCE_HCLK_8		1
#define SAMD21_SYSTICK_CSR_COUNTFLAG	16

#define SAMD21_SYSTICK_PRI		15

/* The NVIC starts at 0xe000e100, so add that to the offsets to find the absolute address */

struct samd21_nvic {
	vuint32_t	iser;		/* 0x000 0xe000e100 Set Enable Register */

	uint8_t		_unused020[0x080 - 0x004];

	vuint32_t	icer;		/* 0x080 0xe000e180 Clear Enable Register */

	uint8_t		_unused0a0[0x100 - 0x084];

	vuint32_t	ispr;		/* 0x100 0xe000e200 Set Pending Register */

	uint8_t		_unused120[0x180 - 0x104];

	vuint32_t	icpr;		/* 0x180 0xe000e280 Clear Pending Register */

	uint8_t		_unused1a0[0x300 - 0x184];

	vuint32_t	ipr[8];		/* 0x300 0xe000e400 Priority Register */
};

extern struct samd21_nvic samd21_nvic;

#define samd21_nvic (*(struct samd21_nvic *) 0xe000e100)

#define SAMD21_NVIC_ISR_PM_POS		0
#define SAMD21_NVIC_ISR_SYSCTRL_POS	1
#define SAMD21_NVIC_ISR_WDT_POS		2
#define SAMD21_NVIC_ISR_RTC_POS		3
#define SAMD21_NVIC_ISR_EIC_POS		4
#define SAMD21_NVIC_ISR_NVMCTRL_POS	5
#define SAMD21_NVIC_ISR_DMAC_POS	6
#define SAMD21_NVIC_ISR_USB_POS		7
#define SAMD21_NVIC_ISR_EVSYS_POS	8
#define SAMD21_NVIC_ISR_SERCOM0_POS	9
#define SAMD21_NVIC_ISR_SERCOM1_POS	10
#define SAMD21_NVIC_ISR_SERCOM2_POS	11
#define SAMD21_NVIC_ISR_SERCOM3_POS	12
#define SAMD21_NVIC_ISR_SERCOM4_POS	13
#define SAMD21_NVIC_ISR_SERCOM5_POS	14
#define SAMD21_NVIC_ISR_TCC0_POS	15
#define SAMD21_NVIC_ISR_TCC1_POS	16
#define SAMD21_NVIC_ISR_TCC2_POS	17
#define SAMD21_NVIC_ISR_TC3_POS		18
#define SAMD21_NVIC_ISR_TC4_POS		19
#define SAMD21_NVIC_ISR_TC5_POS		20
#define SAMD21_NVIC_ISR_TC6_POS		21
#define SAMD21_NVIC_ISR_TC7_POS		22
#define SAMD21_NVIC_ISR_ADC_POS		23
#define SAMD21_NVIC_ISR_AC_POS		24
#define SAMD21_NVIC_ISR_DAC_POS		25
#define SAMD21_NVIC_ISR_PTC_POS		26
#define SAMD21_NVIC_ISR_I2S_POS		27
#define SAMD21_NVIC_ISR_AC1_POS		28
#define SAMD21_NVIC_ISR_TCC3_POS	29

#define IRQ_MASK(irq)	(1 << (irq))
#define IRQ_BOOL(v,irq)	(((v) >> (irq)) & 1)

static inline void
samd21_nvic_set_enable(int irq) {
	samd21_nvic.iser = IRQ_MASK(irq);
}

static inline void
samd21_nvic_clear_enable(int irq) {
	samd21_nvic.icer = IRQ_MASK(irq);
}

static inline int
samd21_nvic_enabled(int irq) {
	return IRQ_BOOL(samd21_nvic.iser, irq);
}

static inline void
samd21_nvic_set_pending(int irq) {
	samd21_nvic.ispr = IRQ_MASK(irq);
}

static inline void
samd21_nvic_clear_pending(int irq) {
	samd21_nvic.icpr = IRQ_MASK(irq);
}

static inline int
samd21_nvic_pending(int irq) {
	return IRQ_BOOL(samd21_nvic.ispr, irq);
}

#define IRQ_PRIO_REG(irq)	((irq) >> 2)
#define IRQ_PRIO_BIT(irq)	(((irq) & 3) << 3)
#define IRQ_PRIO_MASK(irq)	(0xff << IRQ_PRIO_BIT(irq))

static inline void
samd21_nvic_set_priority(int irq, uint8_t prio) {
	int		n = IRQ_PRIO_REG(irq);
	uint32_t	v;

	v = samd21_nvic.ipr[n];
	v &= ~IRQ_PRIO_MASK(irq);
	v |= (prio) << IRQ_PRIO_BIT(irq);
	samd21_nvic.ipr[n] = v;
}

static inline uint8_t
samd21_nvic_get_priority(int irq) {
	return (samd21_nvic.ipr[IRQ_PRIO_REG(irq)] >> IRQ_PRIO_BIT(irq)) & IRQ_PRIO_MASK(0);
}



/* Cortex M0+ SCB */

struct samd21_scb {
	vuint32_t	cpuid;
	vuint32_t	icsr;
	vuint32_t	vtor;
	vuint32_t	aircr;

	vuint32_t	scr;
	vuint32_t	ccr;
	vuint32_t	shpr1;
	vuint32_t	shpr2;

	vuint32_t	shpr3;
	vuint32_t	shcrs;
	vuint32_t	cfsr;
	vuint32_t	hfsr;

	uint32_t	unused_30;
	vuint32_t	mmfar;
	vuint32_t	bfar;
};

extern struct samd21_scb samd21_scb;

#define samd21_scb (*(struct samd21_scb *) 0xe000ed00)

#define SAMD21_SCB_AIRCR_VECTKEY	16
#define  SAMD21_SCB_AIRCR_VECTKEY_KEY		0x05fa
#define SAMD21_SCB_AIRCR_PRIGROUP	8
#define SAMD21_SCB_AIRCR_SYSRESETREQ	2
#define SAMD21_SCB_AIRCR_VECTCLRACTIVE	1
#define SAMD21_SCB_AIRCR_VECTRESET	0

/* The NVM Calibration and auxiliary space starts at 0x00800000 */

struct samd21_aux0 {
	vuint64_t	userrow;
};

extern struct samd21_aux0 samd21_aux0;

#define samd21_aux0 (*(struct samd21_aux0 *) 0x00804000)

#define SAMD21_AUX0_USERROW_BOOTPROT	0
#define SAMD21_AUX0_USERROW_EEPROM		4
#define SAMD21_AUX0_USERROW_BOD33_LEVEL	8
#define SAMD21_AUX0_USERROW_BOD33_ENABLE	14
#define SAMD21_AUX0_USERROW_BOD33_ACTION	15
#define SAMD21_AUX0_USERROW_WDT_ENABLE	25
#define SAMD21_AUX0_USERROW_WDT_ALWAYS_ON	26
#define SAMD21_AUX0_USERROW_WDT_PERIOD	27
#define SAMD21_AUX0_USERROW_WDT_WINDOW	31
#define SAMD21_AUX0_USERROW_WDT_EWOFFSET	35
#define SAMD21_AUX0_USERROW_WDT_WEN		39
#define SAMD21_AUX0_USERROW_BOD33_HYST	40
#define SAMD21_AUX0_USERROW_LOCK		48

struct samd21_aux1 {
	vuint64_t	reserved_00;
	vuint64_t	device_config;

	vuint64_t	reserved_10;
	vuint64_t	reserved_18;

	vuint64_t	calibration;
	vuint64_t	reserved_28;
};

extern struct samd21_aux1 samd21_aux1;

#define samd21_aux1 (*(struct samd21_aux1 *) 0x00806000)

#define SAMD21_AUX1_CALIBRATION_ADC_LINEARITY	27
#define  SAMD21_AUX1_CALIBRATION_ADC_LINEARITY_MASK	0xff
#define SAMD21_AUX1_CALIBRATION_ADC_BIASCAL	35
#define  SAMD21_AUX1_CALIBRATION_ADC_BIASCAL_MASK	0x7
#define SAMD21_AUX1_CALIBRATION_OSC32K_CAL	38
#define SAMD21_AUX1_CALIBRATION_USB_TRANSN	45
#define  SAMD21_AUX1_CALIBRATION_USB_TRANSN_MASK	0x1f
#define SAMD21_AUX1_CALIBRATION_USB_TRANSP	50
#define  SAMD21_AUX1_CALIBRATION_USB_TRANSP_MASK	0x1f
#define SAMD21_AUX1_CALIBRATION_USB_TRIM	55
#define  SAMD21_AUX1_CALIBRATION_USB_TRIM_MASK		0x07
#define SAMD21_AUX1_CALIBRATION_DFLL48M_COARSE_CAL	58
#define  SAMD21_AUX1_CALIBRATION_DFLL48M_COARSE_CAL_MASK	0x3f

struct samd21_serial {
	vuint32_t	reserved_00;
	vuint32_t	reserved_04;
	vuint32_t	reserved_08;
	vuint32_t	word0;

	vuint32_t	reserved_10;
	vuint32_t	reserved_14;
	vuint32_t	reserved_18;
	vuint32_t	reserved_1c;

	vuint32_t	reserved_20;
	vuint32_t	reserved_24;
	vuint32_t	reserved_28;
	vuint32_t	reserved_2c;

	vuint32_t	reserved_30;
	vuint32_t	reserved_34;
	vuint32_t	reserved_38;
	vuint32_t	reserved_3c;

	vuint32_t	word1;
	vuint32_t	word2;
	vuint32_t	word3;
	vuint32_t	reserved_4c;
};

extern struct samd21_serial samd21_serial;

#define samd21_serial (*(struct samd21_serial *) 0x0080a000)

static inline void
samd21_gclk_wait_sync(void)
{
	while (samd21_gclk.status & (1 << SAMD21_GCLK_STATUS_SYNCBUSY))
		;
}

static inline void
samd21_dfll_wait_sync(void)
{
	while ((samd21_sysctrl.pclksr & (1 << SAMD21_SYSCTRL_PCLKSR_DFLLRDY)) == 0)
		;
}

static inline void
samd21_gclk_gendiv(uint32_t id, uint32_t div)
{
	samd21_gclk.gendiv = ((id << SAMD21_GCLK_GENDIV_ID) |
			      (div << SAMD21_GCLK_GENDIV_DIV));
	samd21_gclk_wait_sync();
}

static inline void
samd21_gclk_genctrl(uint32_t src, uint32_t id)
{
	samd21_gclk.genctrl = ((id << SAMD21_GCLK_GENCTRL_ID) |
			       (src << SAMD21_GCLK_GENCTRL_SRC) |
			       (0 << SAMD21_GCLK_GENCTRL_OE) |
			       (1 << SAMD21_GCLK_GENCTRL_GENEN));
	samd21_gclk_wait_sync();
}

static inline void
samd21_gclk_clkctrl(uint32_t gen, uint32_t id)
{
	samd21_gclk.clkctrl = ((gen << SAMD21_GCLK_CLKCTRL_GEN) |
			       (id << SAMD21_GCLK_CLKCTRL_ID) |
			       (1 << SAMD21_GCLK_CLKCTRL_CLKEN));
	samd21_gclk_wait_sync();
}

#define isr_decl(name) \
	void samd21_ ## name ## _isr(void)

isr_decl(halt);
isr_decl(ignore);
isr_decl(nmi);
isr_decl(hardfault);
isr_decl(memmanage);
isr_decl(busfault);
isr_decl(usagefault);
isr_decl(svc);
isr_decl(debugmon);
isr_decl(pendsv);
isr_decl(systick);
isr_decl(pm);		/* IRQ0 */
isr_decl(sysctrl);
isr_decl(wdt);
isr_decl(rtc);
isr_decl(eic);
isr_decl(nvmctrl);
isr_decl(dmac);
isr_decl(usb);
isr_decl(evsys);
isr_decl(sercom0);
isr_decl(sercom1);
isr_decl(sercom2);
isr_decl(sercom3);
isr_decl(sercom4);
isr_decl(sercom5);
isr_decl(tcc0);
isr_decl(tcc1);
isr_decl(tcc2);
isr_decl(tc3);
isr_decl(tc4);
isr_decl(tc5);
isr_decl(tc6);
isr_decl(tc7);
isr_decl(adc);
isr_decl(ac);
isr_decl(dac);
isr_decl(ptc);
isr_decl(i2s);
isr_decl(ac1);
isr_decl(tcc3);

#undef isr_decl

#endif /* _SAMD21_H_ */
