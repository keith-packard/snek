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

SNEK_ROOT = ../..
SNEK_AVR = $(SNEK_ROOT)/chips/avr

PROGNAME=snek-uduino
PRODUCT_NAME=snek-uduino

AVR_CLOCK=16000000UL

include $(SNEK_AVR)/snek-avr.defs

SNEK_LOCAL_SRC = \
	$(SNEK_AVR_SRC)

SNEK_LOCAL_INC = \
	$(SNEK_AVR_INC)

SNEK_LOCAL_BUILTINS = \
	$(SNEK_AVR_BUILTINS) \
	snek-uduino.builtin

include $(SNEK_ROOT)/snek-install.defs

all: $(ELF) $(HEX) snek-uduino-install

$(PROG): Makefile $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SNEK_OBJ) $(LDFLAGS)
	@gawk '/__data_load_end/{printf("ROM used: %d bytes\n", strtonum($$1)); }' $(MAP)

snek-uduino-install: snek-uduino-install.in
	$(SNEK_SED) $^ > $@
	chmod +x $@

install:: $(HEX)
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(HEX) $(DESTDIR)$(SHAREDIR)

install:: snek-uduino-install snek-uduino-install.1
	install -d $(DESTDIR)$(BINDIR)
	install snek-uduino-install $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(MANDIR)/man1
	install -m 0644 snek-uduino-install.1 $(DESTDIR)$(MANDIR)/man1

clean::
	rm -f snek-uduino-install
	rm -f *.hex *.elf *.map
	rm -f ao-product.h

uninstall::

ISP=avrisp2

load: $(HEX) snek-uduino-install
	./snek-uduino-install -quick -isp $(ISP) -hex $(HEX) load
