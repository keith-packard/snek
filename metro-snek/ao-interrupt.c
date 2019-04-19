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

#include <ao.h>

extern void main(void);
extern char __stack__;
extern char __text_start__, __text_end__;
extern char __data_start__, __data_end__;
extern char __bss_start__, __bss_end__;

/* Interrupt functions */

void samd21_halt_isr(void)
{
	ao_panic(AO_PANIC_CRASH);
}

void samd21_ignore_isr(void)
{
}

const void *samd21_interrupt_vector[];

uint32_t
samd21_flash_size(void)
{
	uint32_t	nvmp = (samd21_nvmctrl.param >> SAMD21_NVMCTRL_PARAM_NVMP) & SAMD21_NVMCTRL_PARAM_NVMP_MASK;
	uint32_t	psz = (samd21_nvmctrl.param >> SAMD21_NVMCTRL_PARAM_PSZ) & SAMD21_NVMCTRL_PARAM_PSZ_MASK;

	/* page size is 2**(3 + psz) */
	return nvmp << (3 + psz);
}

void start(void)
{
	/* Turn on sysctrl */
	samd21_pm.apbamask |= (1 << SAMD21_PM_APBAMASK_SYSCTRL);

	samd21_scb.vtor = (uint32_t) &samd21_interrupt_vector;
	memcpy(&__data_start__, &__text_end__, &__data_end__ - &__data_start__);
	memset(&__bss_start__, '\0', &__bss_end__ - &__bss_start__);
	main();
}

#define STRINGIFY(x) #x

#define isr(name) \
	void __attribute__ ((weak)) samd21_ ## name ## _isr(void); \
	_Pragma(STRINGIFY(weak samd21_ ## name ## _isr = samd21_ignore_isr))

#define isr_halt(name) \
	void __attribute__ ((weak)) samd21_ ## name ## _isr(void); \
	_Pragma(STRINGIFY(weak samd21_ ## name ## _isr = samd21_halt_isr))

isr(nmi);
isr_halt(hardfault);
isr_halt(memmanage);
isr_halt(busfault);
isr_halt(usagefault);
isr(svc);
isr(debugmon);
isr(pendsv);
isr(systick);
isr(pm);		/* IRQ0 */
isr(sysctrl);
isr(wdt);
isr(rtc);
isr(eic);
isr(nvmctrl);
isr(dmac);
isr(usb);
isr(evsys);
isr(sercom0);
isr(sercom1);
isr(sercom2);
isr(sercom3);
isr(sercom4);
isr(sercom5);
isr(tcc0);
isr(tcc1);
isr(tcc2);
isr(tc3);
isr(tc4);
isr(tc5);
isr(tc6);
isr(tc7);
isr(adc);
isr(ac);
isr(dac);
isr(ptc);
isr(i2s);
isr(ac1);
isr(tcc3);

#undef isr
#undef isr_halt

#define i(addr,name)	[(addr)/4] = samd21_ ## name ## _isr

__attribute__ ((section(".interrupt")))
const void *samd21_interrupt_vector[] __attribute((aligned(128))) = {
	[0] = &__stack__,
	[1] = start,
	i(0x08, nmi),
	i(0x0c, hardfault),
	i(0x2c, svc),
	i(0x30, debugmon),
	i(0x38, pendsv),
	i(0x3c, systick),

	i(0x40, pm),		/* IRQ0 */
	i(0x44, sysctrl),
	i(0x48, wdt),
	i(0x4c, rtc),
	i(0x50, eic),
	i(0x54, nvmctrl),
	i(0x58, dmac),
	i(0x5c, usb),
	i(0x60, evsys),
	i(0x64, sercom0),
	i(0x68, sercom1),
	i(0x6c, sercom2),
	i(0x70, sercom3),
	i(0x74, sercom4),
	i(0x78, sercom5),
	i(0x7c, tcc0),
	i(0x80, tcc1),
	i(0x84, tcc2),
	i(0x88, tc3),
	i(0x8c, tc4),
	i(0x90, tc5),
	i(0x94, tc6),
	i(0x98, tc7),
	i(0x9c, adc),
	i(0xa0, ac),
	i(0xa4, dac),
	i(0xa8, ptc),
	i(0xac, i2s),
	i(0xb0, ac1),
	i(0xb4, tcc3),
};
