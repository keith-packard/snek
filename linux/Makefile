#
# Copyright © 2019 Keith Packard <keithp@keithp.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#

SNEK_ROOT=..

SNEK_NO_BUILD_TARGETS = 1

include ../snek-install.defs

LINUX_DIST=snek-linux-$(SNEK_VERSION).tar.bz2
LINUX_SH=snek-linux-$(SNEK_VERSION).sh
DESKTOP_FILES=../posix/snek.desktop.in ../snekde/snekde.desktop.in
PROGS=../posix/snek ../snekde/snekde ../snek-duino/snek-duino-install.in
EXAMPLES=../examples
ICONS=../snek.svg
DOC=../doc/snek.pdf ../snek-duino/snek-duino-install.1 ../snekde/snekde.1 ../COPYING
LINUX_FILES=$(PROGS) $(DESKTOP_FILES) $(FIRMWARE) $(ICONS) $(DOC) $(EXAMPLES)

all: $(LINUX_SH)

$(LINUX_SH): $(LINUX_DIST) linux-install.sh
	cat linux-install.sh $(LINUX_DIST) > $@
	chmod +x $@

$(LINUX_DIST): $(LINUX_FILES) $(LINUX_EXTRA)
	-rm -f $@
	-rm -rf linux
	mkdir -p linux/Snek
	cp -a $(LINUX_FILES) linux/Snek
	tar cjf $@ -C linux Snek

upload: $(LINUX_SH)
	rsync -avz $(LINUX_SH) $(SNEKDIST)

clean:
	rm -f snek-*.sh *.bz2
	rm -rf linux
