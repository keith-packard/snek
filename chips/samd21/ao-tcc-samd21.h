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

#ifndef _AO_TCC_SAMD21_H_
#define _AO_TCC_SAMD21_H_

void
ao_tcc_set(struct samd21_tcc *tcc, uint8_t channel, uint32_t value);

void
ao_tcc_samd21_init(void);

#endif /* _AO_TCC_SAMD21_H_ */
