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
BOARD?=nano-every
CBOARD?=NanoEvery
UBOARD?=NANOEVERY
PORT?=/dev/ttyACM0

include $(SNEK_ATMEGA)/snek-atmega.defs

SNEK_LOCAL_VPATH = $(SNEK_ATMEGA)

SNEK_LOCAL_SRC = \
	snek-pow.c \
	snek-nano-every.c \
	$(SNEK_ATMEGA_SRC)

SNEK_LOCAL_INC = \
	$(SNEK_ATMEGA_INC)

SNEK_LOCAL_BUILTINS = \
	$(SNEK_ATMEGA_BUILTINS) \
	snek-nano-every.builtin

include $(SNEK_ROOT)/snek-install.defs

AVR_ROOT=$(HOME)/misc/arduino/arduino-1.8.13/hardware/tools/avr

SNEK_NANO_EVERY_SED=$(SNEK_SED) \
	-e 's;@BOARD@;$(BOARD);g' \
	-e 's;@UBOARD@;$(UBOARD);g' \
	-e 's;@CBOARD@;$(CBOARD);g' \
	-e 's;@PORT@;$(PORT);g' \
	-e 's;@AVR_ROOT@;$(AVR_ROOT);g'

SNEK_CFLAGS = $(SNEK_MOST_WARNINGS) $(SNEK_BASE_CFLAGS)

BASE=snek-$(BOARD)-$(SNEK_VERSION)
ELF=$(BASE).elf
HEX=$(BASE).hex
MAP=$(BASE).map
CC=$(AVR_ROOT)/bin/avr-gcc

OPT=-Os -frename-registers -funsigned-char -fno-jump-tables -mcall-prologues

CFLAGS=$(OPT) -DF_CPU=20000000UL -mmcu=atmega4809 -I. -I$(SNEK_LOCAL_VPATH) -g $(SNEK_CFLAGS) -Waddr-space-convert
LDFLAGS=$(SNEK_LDFLAGS) \
	-Wl,-uvfprintf -lprintf_flt -lm \
	-Wl,--defsym -Wl,__TEXT_REGION_LENGTH__=0xc000 \
	-Wl,--defsym -Wl,__DATA_REGION_LENGTH__=0x1780 \
	-Wl,--defsym -Wl,__EEPROM_REGION_LENGTH__=0x100 \
	-Wl,-Map=$(MAP)

all: $(HEX) snek-$(BOARD)-install snek-$(BOARD)-install.1 snek-$(BOARD)-reset-port

$(HEX): $(ELF)
	avr-objcopy -O ihex -R .eeprom $^ $@

$(ELF): $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@gawk '/__data_load_end/{printf("ROM used: %d bytes\n", strtonum($$1)); }' $(MAP)

snek-$(BOARD)-install: snek-nano-every-install.in
	$(SNEK_NANO_EVERY_SED) $^ > $@
	chmod +x $@

load: $(HEX) snek-$(BOARD)-install snek-$(BOARD)-reset-port
	./snek-$(BOARD)-install -reset ./snek-$(BOARD)-reset-port -hex $(HEX)

snek-$(BOARD)-install.1: snek-nano-every-install.1.in
	$(SNEK_NANO_EVERY_SED) $^ > $@

snek-$(BOARD)-reset-port: snek-nano-every-reset-port.c
	gcc -o $@ $^

install: snek-$(BOARD)-install $(HEX) snek-$(BOARD)-install.1
	install -d $(DESTDIR)$(BINDIR)
	install snek-$(BOARD)-install $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(HEX) $(DESTDIR)$(SHAREDIR)
	install -d $(DESTDIR)$(MANDIR)/man1
	install -m 0644 snek-$(BOARD)-install.1 $(DESTDIR)$(MANDIR)/man1

clean::
	rm -f *.elf *.hex *.map snek-$(BOARD)-install snek-$(BOARD)-install.1 snek-$(BOARD)-reset-port
