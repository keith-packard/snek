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
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <ao.h>
#include <ao-dma-samd21.h>

static struct samd21_dmac_desc samd21_dmac_desc[SAMD21_DMAC_NCHAN]  __attribute__((aligned(16)));
static struct samd21_dmac_desc samd21_dmac_wrb[SAMD21_DMAC_NCHAN]  __attribute__((aligned(16)));

static volatile uint16_t	saved_intpend;
static volatile int		interrupts;

static struct {
	void (*callback)(uint8_t id, void *closure);
	void *closure;
} dmac_callback[SAMD21_DMAC_NCHAN];

void
samd21_dmac_isr(void)
{
	uint16_t	intpend = samd21_dmac.intpend;

	++interrupts;
	saved_intpend = intpend;
	if (intpend & 0xff00) {
		uint8_t	id = (intpend >> SAMD21_DMAC_INTPEND_ID) & SAMD21_DMAC_INTPEND_ID_MASK;
		samd21_dmac.intpend = intpend;
		if (intpend & (1 << SAMD21_DMAC_INTPEND_TCMPL)) {
			if (dmac_callback[id].callback)
				(*dmac_callback[id].callback)(id, dmac_callback[id].closure);
		}
	}
}

void
ao_dma_dump(char *where)
{
	printf("DMA %s ctrl %04x intpend %04x intstatus %04x\n",
	       where,
	       samd21_dmac.ctrl,
	       samd21_dmac.intpend,
	       samd21_dmac.intstatus);
	fflush(stdout);
	printf(" busych %04x pendch %04x active %08x chctrla %02x\n",
	       samd21_dmac.busych,
	       samd21_dmac.pendch,
	       samd21_dmac.active,
	       samd21_dmac.chctrla);
	fflush(stdout);
	printf(" chctrlb %08x chintflag %02x chstatus %02x\n",
	       samd21_dmac.chctrlb,
	       samd21_dmac.chintflag,
	       samd21_dmac.chstatus);
	fflush(stdout);
	printf(" btctrl %04x btcnt %04x srcaddr %08x dstaddr %08x descaddr %08x\n",
	       samd21_dmac_desc[0].btctrl,
	       samd21_dmac_desc[0].btcnt,
	       samd21_dmac_desc[0].srcaddr,
	       samd21_dmac_desc[0].dstaddr,
	       samd21_dmac_desc[0].descaddr);
	fflush(stdout);
	printf("intpend %04x interrupts %d\n", saved_intpend, interrupts);
}

void
_ao_dma_start_transfer(uint8_t		id,
		       void		*src,
		       void		*dst,
		       uint16_t		count,
		       uint32_t		chctrlb,
		       uint16_t		btctrl,
		       void		(*callback)(uint8_t id, void *closure),
		       void		*closure)
{
	/* Set up the callback */
	dmac_callback[id].closure = closure;
	dmac_callback[id].callback = callback;

	/* Set up the descriptor */
	samd21_dmac_desc[id].btctrl = btctrl;
	samd21_dmac_desc[id].btcnt = count;
	samd21_dmac_desc[id].srcaddr = (uint32_t) src;
	samd21_dmac_desc[id].dstaddr = (uint32_t) dst;
	samd21_dmac_desc[id].descaddr = 0;

	/* Configure the channel and enable it */
	samd21_dmac.chid = id;
	samd21_dmac.chctrlb = chctrlb;
	samd21_dmac.chctrla = (1 << SAMD21_DMAC_CHCTRLA_ENABLE);
}

void
_ao_dma_done_transfer(uint8_t id)
{
	/* Disable channel */
	samd21_dmac.chid = id;
	samd21_dmac.chctrla = 0;
}

void
ao_dma_init(void)
{
	uint8_t	ch;

	/* Enable DMAC clocks */
	samd21_pm.ahbmask |= (1 << SAMD21_PM_AHBMASK_DMAC);
	samd21_pm.apbbmask |= (1 << SAMD21_PM_APBBMASK_DMAC);

#if 0
	/* Enable HPB clocks so we can talk to peripherals */
	samd21_pm.ahbmask |= ((1 << SAMD21_PM_AHBMASK_HPB0) |
			      (1 << SAMD21_PM_AHBMASK_HPB1) |
			      (1 << SAMD21_PM_AHBMASK_HPB2));
#endif

	samd21_pm.apbamask |= (1 << SAMD21_PM_APBAMASK_PAC0);
	samd21_pm.apbbmask |= (1 << SAMD21_PM_APBBMASK_PAC1);
	samd21_pm.apbcmask |= (1 << SAMD21_PM_APBCMASK_PAC2);

	/* Reset DMAC device */
	samd21_dmac.ctrl = 0;
	samd21_dmac.ctrl = (1 << SAMD21_DMAC_CTRL_SWRST);
	while (samd21_dmac.ctrl & (1 << SAMD21_DMAC_CTRL_SWRST))
		;

	samd21_dmac.baseaddr = (uint32_t) &samd21_dmac_desc[0];
	samd21_dmac.wrbaddr = (uint32_t) &samd21_dmac_wrb[0];

	samd21_dmac.swtrigctrl = 0;

	/* Set QoS to highest value */
	samd21_dmac.qosctrl = ((SAMD21_DMAC_QOSCTRL_HIGH << SAMD21_DMAC_QOSCTRL_DQOS) |
			       (SAMD21_DMAC_QOSCTRL_HIGH << SAMD21_DMAC_QOSCTRL_FQOS) |
			       (SAMD21_DMAC_QOSCTRL_HIGH << SAMD21_DMAC_QOSCTRL_WRBQOS));

	/* Enable DMAC controller with all priority levels */
	samd21_dmac.ctrl = ((1 << SAMD21_DMAC_CTRL_DMAENABLE) |
			    (1 << SAMD21_DMAC_CTRL_LVLEN(0)) |
			    (1 << SAMD21_DMAC_CTRL_LVLEN(1)) |
			    (1 << SAMD21_DMAC_CTRL_LVLEN(2)) |
			    (1 << SAMD21_DMAC_CTRL_LVLEN(3)));

	/* Reset all DMAC channels */
	for (ch = 0; ch < SAMD21_DMAC_NCHAN; ch++) {
		samd21_dmac.chid = ch;
		samd21_dmac.chctrla = (1 << SAMD21_DMAC_CHCTRLA_SWRST);
		while (samd21_dmac.chctrla & (1 << SAMD21_DMAC_CHCTRLA_SWRST))
			;
		samd21_dmac.chintenset = (1 << SAMD21_DMAC_CHINTFLAG_TCMPL);
	}

	/* configure interrupts */
	samd21_nvic_set_enable(SAMD21_NVIC_ISR_DMAC_POS);
	samd21_nvic_set_priority(SAMD21_NVIC_ISR_DMAC_POS, 3);
}
