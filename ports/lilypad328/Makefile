#
# Copyright © 2020 Keith Packard <keithp@keithp.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#

vpath %.in ../duemilanove

BOARD=lilypad328
CBOARD=LilyPad328
UBOARD=LILYPAD328
PORT=/dev/ttyACM0

include ../duemilanove/Makefile

SNEK_CFLAGS = $(SNEK_MOST_WARNINGS) $(SNEK_BASE_CFLAGS) -DUART_BAUD=38400

CFLAGS=$(OPT) -DF_CPU=8000000UL -mmcu=atmega328p -I. -I$(SNEK_LOCAL_VPATH) -g $(SNEK_CFLAGS) -Waddr-space-convert

all:: snek-$(BOARD)-install.in

snek-$(BOARD)-install.in: snek-duemilanove-install.in
	sed -e 's;@BOARD@;$(BOARD);g' \
	-e 's;@UBOARD@;$(UBOARD);g' \
	-e 's;@CBOARD@;$(CBOARD);g' \
	-e 's;@PORT@;$(PORT);g' $^ > $@
