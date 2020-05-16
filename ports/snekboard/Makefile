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

SNEK_SAMD21 = $(SNEK_ROOT)/chips/samd21

PROGNAME=snek-board
PRODUCT_NAME=snekboard
IDPRODUCT=0x804D
IDVENDOR=0x239A

BOOTLOADER_BIN=bootloader-snekboard.bin

include $(SNEK_SAMD21)/snek-samd21.defs

SNEK_LOCAL_SRC = \
	$(SNEK_SAMD21_SRC)

SNEK_LOCAL_INC = \
	$(SNEK_SAMD21_INC)

SNEK_LOCAL_BUILTINS = \
	$(SNEK_SAMD21_BUILTINS) \
	snek-board.builtin

include $(SNEK_ROOT)/snek-install.defs

SRC=$(SNEK_SRC)
OBJ=$(SRC:.c=.o)

all: $(PROG) $(BIN) $(UF2) snek-board-install

$(PROG): Makefile $(OBJ) $(SNEK_SAMD21)/snek-samd21.ld
	$(call quiet,CC) $(LDFLAGS) -Wl,-M=$(MAP) -o $@ $(OBJ) $(LIBS)

$(OBJ): $(SNEK_INC)

distclean:	clean

clean::
	rm -f *.o $(PROGNAME)*.elf $(PROGNAME)*.bin $(PROGNAME)*.uf2 $(PROGNAME)*.map ao-product.h
	rm -f snek-board-install

snek-board-install: snek-board-install.in
	$(SNEK_SED) $^ > $@
	chmod +x $@

install: $(PROG) $(UF2) $(BOOTLOADER_BIN) snek-board-install snek-board-install.1
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(PROG) $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(UF2) $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(BOOTLOADER_BIN) $(DESTDIR)$(SHAREDIR)
	install -d $(DESTDIR)$(BINDIR)
	install snek-board-install $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(MANDIR)/man1
	install -m 0644 snek-board-install.1 $(DESTDIR)$(MANDIR)/man1

uninstall:
