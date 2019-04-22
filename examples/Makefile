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

SNEK_NO_BUILD_TARGETS = 1
SNEK_ROOT = ..

include $(SNEK_ROOT)/snek-install.defs

all:
	echo all done

EXAMPLES = \
	blink.py \
	hanoi.py \
	morse.py \
	snek-bsd.py \
	snek.py \
	track-light.py \
	badshuffle.py \
	goodshuffle.py

install:
	install -d "$(DESTDIR)$(EXAMPLEDIR)"
	for example in $(EXAMPLES); do install -m 0644 "$$example" "$(DESTDIR)$(EXAMPLEDIR)"; done
