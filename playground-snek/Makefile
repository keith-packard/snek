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

SNEK_ROOT = ..

SNEK_SAMD21 = $(SNEK_ROOT)/samd21

include $(SNEK_SAMD21)/snek-samd21.defs


SNEK_LOCAL_SRC = \
	snek-temperature.c \
	$(SNEK_SAMD21_SRC)

SNEK_LOCAL_INC = \
	$(SNEK_SAMD21_INC)

SNEK_LOCAL_BUILTINS = \
	$(SNEK_SAMD21_BUILTINS) \
	snek-temperature.builtin \
	playground-snek.builtin

include $(SNEK_ROOT)/snek-install.defs

PROGNAME=playground-snek
PROG=$(PROGNAME)-$(SNEK_VERSION).elf
BIN=$(PROGNAME)-$(SNEK_VERSION).bin
UF2=$(PROGNAME)-$(SNEK_VERSION).uf2
MAP=$(PROGNAME)-$(SNEK_VERSION).map

PRODUCT=PlaygroundSnek-v0.0
IDPRODUCT=0x000a

SRC=$(SNEK_SRC)
OBJ=$(SRC:.c=.o)

show:
	echo AO_VPATH $(AO_VPATH)

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

ao-product.h: ../snek.defs Makefile
	python3 $(SNEK_SAMD21)/make-product.py -v $(SNEK_VERSION) -p PlaygroundSnek > $@
