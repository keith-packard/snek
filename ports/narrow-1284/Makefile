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
SNEK_ATMEGA = $(SNEK_ROOT)/chips/atmega

include $(SNEK_ATMEGA)/snek-atmega.defs

SNEK_LOCAL_VPATH = $(SNEK_ATMEGA)

SNEK_LOCAL_SRC = \
	snek-narrow-1284.c \
	$(SNEK_ATMEGA_SRC) \
	$(SNEK_ATMEGA_MATH_SRC) \
	snek-input.c

SNEK_LOCAL_INC = \
	$(SNEK_ATMEGA_INC) \
	$(SNEK_ATMEGA_MATH_INC)

SNEK_LOCAL_BUILTINS = \
	$(SNEK_ATMEGA_BUILTINS) \
	$(SNEK_ATMEGA_MATH_BUILTINS) \
	snek-math.builtin \
	snek-input.builtin \
	snek-narrow-1284.builtin

include $(SNEK_ROOT)/snek-install.defs

SNEK_CFLAGS = $(SNEK_MOST_WARNINGS) $(SNEK_BASE_CFLAGS)

BASE=snek-narrow-1284-$(SNEK_VERSION)
ELF=$(BASE).elf
HEX=$(BASE).hex
MAP=$(BASE).map
CC=avr-gcc
CFLAGS=-DF_CPU=16000000UL -mmcu=atmega1284 -I. -I$(SNEK_LOCAL_VPATH) -g -fno-jump-tables $(SNEK_CFLAGS) $(OPT) -Waddr-space-convert
LDFLAGS=$(SNEK_LDFLAGS) \
	-Wl,-uvfprintf -lprintf_flt -lm \
	-Wl,-Map=$(MAP)

PORT=/dev/ttyUSB0

all: $(HEX) snek-narrow-1284-install

$(HEX): $(ELF)
	avr-objcopy -O ihex -R .eeprom $^ $@

$(ELF): $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@gawk '/__data_load_end/{printf("$(BASE) ROM: %d bytes\n", strtonum($$1)); }' $(MAP)

snek-narrow-1284-install: snek-narrow-1284-install.in
	$(SNEK_SED) $^ > $@
	chmod +x $@

load: $(HEX) snek-narrow-1284-install
	./snek-narrow-1284-install -port $(PORT) -hex $(HEX)

install: snek-narrow-1284-install $(HEX) $(USBSERIAL_HEX) snek-narrow-1284-install.1
	install -d $(DESTDIR)$(BINDIR)
	install snek-narrow-1284-install $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(HEX) $(DESTDIR)$(SHAREDIR)
	install -d $(DESTDIR)$(MANDIR)/man1
	install -m 0644 snek-narrow-1284-install.1 $(DESTDIR)$(MANDIR)/man1

clean::
	rm -f *.elf snek*.hex *.map snek-narrow-1284-install

load-direct: $(HEX)
	avrdude -pm1284p -carduino -P$(PORT) -b115200 -D -Uflash:w:$(HEX):i 
