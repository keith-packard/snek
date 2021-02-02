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

SNEK_ROOT = ../..
SNEK_ATMEGA = $(SNEK_ROOT)/chips/atmega

include $(SNEK_ATMEGA)/snek-atmega.defs

SNEK_LOCAL_VPATH = $(SNEK_ATMEGA)

SNEK_LOCAL_SRC = \
	snek-pow.c \
	snek-328p.c \
	$(SNEK_ATMEGA_SRC)

SNEK_LOCAL_INC = \
	$(SNEK_ATMEGA_INC)

SNEK_LOCAL_BUILTINS = \
	$(SNEK_ATMEGA_BUILTINS) \
	snek-328p.builtin

include $(SNEK_ROOT)/snek-install.defs

SNEK_CFLAGS = $(SNEK_MOST_WARNINGS) $(SNEK_BASE_CFLAGS) -DUART_BAUD=115200

BASE=snek-uno-$(SNEK_VERSION)
ELF=$(BASE).elf
HEX=$(BASE).hex
MAP=$(BASE).map
CC=avr-gcc

OPT=-Os -frename-registers -funsigned-char -fno-jump-tables -mcall-prologues

CFLAGS=$(OPT) -DF_CPU=16000000UL -mmcu=atmega328p -I. -I$(SNEK_LOCAL_VPATH) -g $(SNEK_CFLAGS) -Waddr-space-convert
LDFLAGS=$(SNEK_LDFLAGS) \
	-Wl,-uvfprintf -lprintf_flt -lm \
	-Wl,--defsym -Wl,__TEXT_REGION_LENGTH__=0x7e00 \
	-Wl,--defsym -Wl,__DATA_REGION_ORIGIN__=0x800100 \
	-Wl,--defsym -Wl,__DATA_REGION_LENGTH__=0x780 \
	-Wl,--defsym -Wl,__EEPROM_REGION_LENGTH__=0x400 \
	-Wl,-Map=$(MAP)

all: $(HEX) snek-uno-install

$(HEX): $(ELF)
	avr-objcopy -O ihex -R .eeprom $^ $@

$(ELF): $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@gawk '/__data_load_end/{printf("ROM used: %d bytes\n", strtonum($$1)); }' $(MAP)

snek-uno-install: snek-uno-install.in
	$(SNEK_SED) $^ > $@
	chmod +x $@

load: $(HEX) snek-uno-install
	./snek-uno-install -hex $(HEX) snek

install: snek-uno-install $(HEX) snek-uno-install.1
	install -d $(DESTDIR)$(BINDIR)
	install snek-uno-install $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(HEX) $(DESTDIR)$(SHAREDIR)
	install -d $(DESTDIR)$(MANDIR)/man1
	install -m 0644 snek-uno-install.1 $(DESTDIR)$(MANDIR)/man1

clean::
	rm -f snek*.elf snek*.hex snek*.map snek-uno-install
