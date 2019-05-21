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

#ifndef _AO_DMA_SAM21_H_
#define _AO_DMA_SAM21_H_

extern uint8_t	ao_dma_done[SAMD21_DMAC_NCHAN];

void
ao_dma_init(void);

void
ao_dma_start_transfer(uint8_t		id,
		      void		*src,
		      void		*dst,
		      uint16_t		count,
		      uint32_t		chctrlb,
		      uint16_t		btctrl);

void
ao_dma_done_transfer(uint8_t id);

#endif /* _AO_DMA_SAM21_H_ */
