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

SNEK_ROOT=../..

SNEK_NO_BUILD_TARGETS = 1

include $(SNEK_ROOT)/snek-install.defs

ZIP_DIST=snek-$(SNEK_VERSION).zip

PROGS= \
	$(SNEK_ROOT)/snekde/snekde

EXAMPLES=$(SNEK_ROOT)/examples

ICONS=$(SNEK_ROOT)/snek.svg

DOC= \
	$(PDF) \
	$(SNEK_ROOT)/COPYING

ZIP_FILES=$(PROGS) $(FIRMWARE) $(USBFIRMWARE) $(ICONS) $(DOC) $(EXAMPLES)

all: $(ZIP_DIST)

$(ZIP_DIST): $(ZIP_FILES)
	-rm -f $@
	-rm -rf zip
	mkdir -p zip/snek
	cp -a $(ZIP_FILES) zip/snek
	cd zip && zip ../$@ -r snek

install:
	echo install done

upload: $(ZIP_DIST)
	rsync -avz $(ZIP_DIST) $(SNEKDIST)

clean:
	rm -f snek-*.zip
	rm -rf zip
