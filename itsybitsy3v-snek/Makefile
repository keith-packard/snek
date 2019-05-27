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
SNEK_AVR = $(SNEK_ROOT)/avr

PROGNAME=itsybitsy3v-snek
PRODUCT_NAME=ItsyBitsySnek

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

SNEK_AO = $(SNEK_ROOT)/ao
SNEK_AVR = $(SNEK_ROOT)/avr

SNEK_LOCAL_VPATH = $(SNEK_AVR):$(SNEK_AO)

SNEK_AVR_SRC = \
	snek-io.c \
	snek-avr-eeprom.c \
	ao-snek-avr.c \
	ao-usb-avr.c \
	ao-notask.c \
	ao-product.c \
	ao-panic.c

SNEK_AVR_INC = \
	ao.h \
	ao-arch.h \
	ao-arch-funcs.h \
	ao-led.h \
	ao-notask.h \
	ao-pins.h \
	ao-product.h \
	snek-avr.h \
	ao-usb.h

SNEK_AVR_BUILTINS = \
	itsybitsy3v.builtin

OPT=-Os

AO_CFLAGS=\
	-std=gnu99 $(WARN_FLAGS) $(OPT) -g

AVR_CFLAGS=-DF_CPU=8000000UL -mmcu=atmega32u4 -g -fno-jump-tables $(SNEK_CFLAGS) -mcall-prologues -Waddr-space-convert \
	-I. -I$(SNEK_AVR) -I$(SNEK_AO) -I$(SNEK_ROOT) $(AO_CFLAGS)

CFLAGS = $(AVR_CFLAGS) $(SNEK_CFLAGS)

LDSCRIPT=$(SNEK_AVR)/snek-avr51.x

LDFLAGS=-Wl,-uvfprintf -lprintf_flt -lm \
	-Wl,--defsym -Wl,__TEXT_REGION_LENGTH__=0x8000 \
	-Wl,--defsym -Wl,__DATA_REGION_LENGTH__=0x880 \
	-Wl,--defsym -Wl,__EEPROM_REGION_LENGTH__=0x400 \
	-Wl,-T$(LDSCRIPT) \
	-Wl,-Map=$(MAP)

CC=avr-gcc

.SUFFIXES: .elf .hex

.elf.hex:
	avr-objcopy -O ihex -R .eeprom $^ $@

ao-product.h: $(SNEK_ROOT)/snek.defs Makefile
	python3 $(SNEK_AO)/make-product.py -v $(SNEK_VERSION) -p $(PRODUCT_NAME) > $@

SNEK_LOCAL_SRC = \
	$(SNEK_AVR_SRC)

SNEK_LOCAL_INC = \
	$(SNEK_AVR_INC)

SNEK_LOCAL_BUILTINS = \
	$(SNEK_AVR_BUILTINS)

include ../snek-install.defs

BASE=$(PROGNAME)-$(SNEK_VERSION)
ELF=$(BASE).elf
HEX=$(BASE).hex
MAP=$(BASE).map

PRODUCT=$(PRODUCT_NAME)-v$(SNEK_VERSION)
IDPRODUCT=0x000a

SNEK_CFLAGS = $(SNEK_MOST_WARNINGS) $(SNEK_BASE_CFLAGS)

SRC=$(SNEK_SRC)
OBJ=$(SRC:.c=.o)

all: $(HEX)

$(HEX): $(ELF)

$(ELF): $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@awk '/__data_load_end/{printf("ROM used: %d bytes\n", strtonum($$1)); }' $(MAP)

echo:
	echo PROG $(PROG) HEX $(HEX) MAP $(MAP) SNEK_OBJ $(SNEK_OBJ)

ISP=avrisp2

load: $(HEX) ../itsybitsy-snek/snek-itsybitsy-install
	./snek-itsybitsy-install -quick -isp $(ISP) -hex $(HEX) load

install: $(HEX)
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(HEX) $(DESTDIR)$(SHAREDIR)

clean::
	rm -f snek-itsybitsy3v-install
	rm -f *.hex *.elf *.map
	rm -f ao-product.h
