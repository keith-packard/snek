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

vpath %.in ../duemilanove

SNEK_ROOT = ../..
SNEK_ATMEGA = $(SNEK_ROOT)/chips/atmega
BOARD?=duemilanove
CBOARD?=Duemilanove
UBOARD?=DUEMILANOVE
PORT?=/dev/ttyUSB0
CLOCK?=16000000
BAUD?=115200
INSTALLER?=1
ATMEGA_FLASH_SIZE?=0x7e00
MCU?=m328p
SNEK_GLOBAL_ALIAS?=n

include $(SNEK_ATMEGA)/snek-atmega.defs

SNEK_LOCAL_VPATH = $(SNEK_ATMEGA)

SNEK_LOCAL_SRC = \
	snek-pow.c \
	snek-328p.c \
	$(SNEK_ATMEGA_SRC_EXTRA) \
	$(SNEK_ATMEGA_SRC)

SNEK_LOCAL_INC = \
	$(SNEK_ATMEGA_INC)

SNEK_LOCAL_BUILTINS = \
	$(SNEK_ATMEGA_BUILTINS) \
	$(SNEK_ATMEGA_BUILTINS_EXTRA) \
	snek-tone.builtin \
	snek-328p.builtin

include $(SNEK_ROOT)/snek-install.defs

SNEK_ATMEGA_SED=$(SNEK_SED) \
	-e 's;@BOARD@;$(BOARD);g' \
	-e 's;@UBOARD@;$(UBOARD);g' \
	-e 's;@CBOARD@;$(CBOARD);g' \
	-e 's;@BAUD@;$(BAUD);g' \
	-e 's;@PORT@;$(PORT);g' \
	-e 's;@MCU@;$(MCU);g'

SNEK_ATMEGA_BOARD_SED = sed \
	-e 's;@SNEKLIB@;$(SNEKLIB);' \
	-e 's/@SNEK_VERSION@/$(SNEK_VERSION)/' \
	-e 's/@SNEK_VERSION_DASH@/$(SNEK_VERSION_DASH)/' \
	-e 's;@SNEK_ROOT@;$(SNEK_ROOT);' \
	-e 's/@SNEK_DATE@/$(SNEK_DATE)/' \
	-e 's;@BOARD@;$(BOARD);g' \
	-e 's;@UBOARD@;$(UBOARD);g' \
	-e 's;@CBOARD@;$(CBOARD);g' \
	-e 's;@BAUD@;$(BAUD);g' \
	-e 's;@PORT@;$(PORT);g' \
	-e 's;@MCU@;$(MCU);g'

SNEK_CFLAGS = $(SNEK_MOST_WARNINGS) $(SNEK_BASE_CFLAGS)

BASE=snek-$(BOARD)-$(SNEK_VERSION)
ELF=$(BASE).elf
HEX=$(BASE).hex
MAP=$(BASE).map
CC=avr-gcc

CFLAGS=-DUART_BAUD=$(BAUD) -DF_CPU=$(CLOCK)UL -mmcu=atmega328p -I. -I$(SNEK_LOCAL_VPATH) $(SNEK_CFLAGS) $(OPT) -g -Waddr-space-convert
LDFLAGS=$(SNEK_LDFLAGS) \
	-lm \
	-Wl,--defsym -Wl,__TEXT_REGION_LENGTH__=$(ATMEGA_FLASH_SIZE) \
	-Wl,-Map=$(MAP)

all:: $(HEX)

$(HEX): $(ELF)
	avr-objcopy -O ihex -R .eeprom $^ $@

$(ELF): $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@gawk '/__data_load_end/{printf("$(BASE) ROM: %d bytes\n", strtonum($$1)); }' $(MAP)

ifeq ($(INSTALLER),1)
all:: snek-$(BOARD)-install snek-$(BOARD)-install.in snek-$(BOARD)-install.1

snek-$(BOARD)-install: snek-atmega328-install.in
	$(SNEK_ATMEGA_SED) $^ > $@
	chmod +x $@

snek-$(BOARD)-install.in: snek-atmega328-install.in
	if [ $@ != $^ ]; then $(SNEK_ATMEGA_BOARD_SED) $^ > $@; fi

load: $(HEX) snek-$(BOARD)-install
	./snek-$(BOARD)-install -hex $(HEX)

snek-$(BOARD)-install.1: snek-duemilanove-install.1.in
	$(SNEK_ATMEGA_SED) $^ > $@

install: snek-$(BOARD)-install $(HEX) snek-$(BOARD)-install.1
	install -d $(DESTDIR)$(BINDIR)
	install snek-$(BOARD)-install $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(HEX) $(DESTDIR)$(SHAREDIR)
	install -d $(DESTDIR)$(MANDIR)/man1
	install -m 0644 snek-$(BOARD)-install.1 $(DESTDIR)$(MANDIR)/man1

endif

clean::
	rm -f *.elf *.hex *.map snek-$(BOARD)-install snek-$(BOARD)-install.in snek-$(BOARD)-install.1
