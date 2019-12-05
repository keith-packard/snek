#
# Copyright © 2019 Keith Packard <keithp@keithp.com>
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

all:
	make -C main all
	./esp32-idf build

clean:
	./esp32-idf clean
	make -C main clean

load:
	./esp32-idf -p /dev/ttyUSB1 flash

connect:
	minicom -D /dev/ttyUSB1 -b 115200
