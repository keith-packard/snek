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

vpath %.in ../duemilanove-big

BOARD?=duemilanove-big
CBOARD?=Duemilanove-big
UBOARD?=DUEMILANOVE-BIG
PORT?=/dev/ttyUSB0
INSTALLER?=0

ATMEGA_FLASH_SIZE=0x8000

SNEK_ATMEGA_SRC_EXTRA = \
	snek-input.c

SNEK_ATMEGA_BUILTINS_EXTRA = \
	snek-input.builtin

include ../duemilanove/Makefile

all:: snek-$(BOARD)-install snek-$(BOARD)-install.1

snek-$(BOARD)-install: snek-duemilanove-big-install.in
	$(SNEK_ATMEGA_SED) $^ > $@
	chmod +x $@

install:: $(HEX)
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(HEX) $(DESTDIR)$(SHAREDIR)

install:: snek-$(BOARD)-install snek-$(BOARD)-install.1
	install -d $(DESTDIR)$(BINDIR)
	install snek-$(BOARD)-install $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(MANDIR)/man1
	install -m 0644 snek-$(BOARD)-install.1 $(DESTDIR)$(MANDIR)/man1

clean::
	rm -f snek-$(BOARD)-install snek-$(BOARD)-install.1
	rm -f *.hex *.elf *.map
	rm -f ao-product.h

uninstall::

ISP=avrisp2

load: $(HEX) snek-$(BOARD)-install
	./snek-$(BOARD)-install -quick -isp $(ISP) -hex $(HEX) load

snek-$(BOARD)-install.1: snek-duemilanove-big-install.1.in
	$(SNEK_ATMEGA_SED) $^ > $@
