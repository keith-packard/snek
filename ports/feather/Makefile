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

PROGNAME=snek-feather
PRODUCT_NAME=SnekFeather

include $(SNEK_SAMD21)/snek-samd21.defs

SNEK_LOCAL_SRC = \
	$(SNEK_SAMD21_SRC)

SNEK_LOCAL_INC = \
	$(SNEK_SAMD21_INC)

SNEK_LOCAL_BUILTINS = \
	$(SNEK_SAMD21_BUILTINS) \
	snek-feather.builtin

include $(SNEK_ROOT)/snek-install.defs

SRC=$(SNEK_SRC)
OBJ=$(SRC:.c=.o)

all: $(ELF) $(BIN) $(UF2)

$(PROG): Makefile $(OBJ)
	$(call quiet,CC) $(LDFLAGS) -Wl,-M=$(MAP) -o $@ $(OBJ) $(LIBS)

$(OBJ): $(SNEK_INC)

distclean:	clean

clean::
	rm -f *.o *.elf *.bin *.uf2 *.map ao-product.h

install:
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(UF2) $(DESTDIR)$(SHAREDIR)

uninstall:
