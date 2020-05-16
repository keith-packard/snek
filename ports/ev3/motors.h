/*
 * Copyright © 2020 Mikhail Gusarov <dottedmag@dottedmag.net>
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
#ifndef _SNEK_EV3_MOTORS_H_
#define _SNEK_EV3_MOTORS_H_

#include "snek.h"

/* read() implementation for motors */
snek_poly_t
snek_ev3_motors_read(snek_poly_t port);

#endif /* _SNEK_EV3_MOTORS_H_ */
