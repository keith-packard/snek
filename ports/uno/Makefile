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

BOARD=uno
CBOARD=Uno
UBOARD=UNO
PORT=/dev/ttyACM0

include ../duemilanove/Makefile

all:: snek-uno-install.in

snek-uno-install.in: snek-duemilanove-install.in
	sed -e 's;@BOARD@;$(BOARD);g' \
	-e 's;@UBOARD@;$(UBOARD);g' \
	-e 's;@CBOARD@;$(CBOARD);g' \
	-e 's;@PORT@;$(PORT);g' $^ > $@
