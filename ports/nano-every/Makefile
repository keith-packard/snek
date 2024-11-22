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

SNEK_ATMEGA_STORE_SRC = snek-atmega-flash.c

include $(SNEK_ATMEGA)/snek-atmega.defs

SNEK_LOCAL_VPATH = $(SNEK_ATMEGA)

SNEK_LOCAL_SRC = \
	snek-pow.c \
	snek-nano-every.c \
	$(SNEK_ATMEGA_MATH_SRC) \
	$(SNEK_ATMEGA_SRC)

SNEK_LOCAL_INC = \
	$(SNEK_ATMEGA_INC) \
	$(SNEK_ATMEGA_MATH_INC)

SNEK_LOCAL_BUILTINS = \
	$(SNEK_ATMEGA_BUILTINS) \
	snek-math-small.builtin \
	snek-tone.builtin \
	snek-nano-every.builtin

include $(SNEK_ROOT)/snek-install.defs

AVR_ROOT=$(HOME)/misc/arduino/arduino-1.8.13/hardware/tools/avr

ATMEGA_CODE_SIZE=0xb000

SNEK_NANO_EVERY_SED=$(SNEK_SED) \
	-e 's;@BOARD@;$(BOARD);g' \
	-e 's;@UBOARD@;$(UBOARD);g' \
	-e 's;@CBOARD@;$(CBOARD);g' \
	-e 's;@ATMEGA_CODE_SIZE@;$(ATMEGA_CODE_SIZE);g' \
	-e 's;@PORT@;$(PORT);g' \
	-e 's;@AVR_ROOT@;$(AVR_ROOT);g'

SNEK_CFLAGS = $(SNEK_MOST_WARNINGS) $(SNEK_BASE_CFLAGS)

BASE=snek-$(BOARD)-$(SNEK_VERSION)
ELF=$(BASE).elf
HEX=$(BASE).hex
MAP=$(BASE).map
CC=avr-gcc

MCU=atmega4809
AVR_LIBC_ROOT=/opt/avr-libc
AVR_LIBC_LIB=$(AVR_LIBC_ROOT)/avr/lib/$(shell avr-gcc -print-multi-os-directory -mmcu=$(MCU))
AVR_LIBC_INCLUDE=$(AVR_LIBC_ROOT)/avr/include
CRT0=crt$(MCU).o

CFLAGS=-DF_CPU=20000000UL \
	-mmcu=$(MCU) \
	-isystem$(AVR_LIBC_INCLUDE) -L$(AVR_LIBC_LIB) \
	-I. -I$(SNEK_LOCAL_VPATH) -g \
	$(SNEK_CFLAGS) \
	$(OPT) \
	-Waddr-space-convert

LDFLAGS=$(SNEK_LDFLAGS) \
	-lm \
	-Wl,--defsym,__TEXT_REGION_LENGTH__=$(ATMEGA_CODE_SIZE) \
	-Wl,-Map=$(MAP)

all: $(HEX) snek-$(BOARD)-install snek-$(BOARD)-install.1 snek-$(BOARD)-reset-port

$(HEX): $(ELF)
	avr-objcopy -O ihex -R .eeprom $^ $@

$(ELF): $(SNEK_OBJ) $(CRT0)
	$(CC) $(CFLAGS) -o $@ $(SNEK_OBJ) $(LDFLAGS)
	@gawk '/__data_load_end/{printf("$(BASE) ROM: %d bytes\n", strtonum($$1)); }' $(MAP)

$(CRT0): $(AVR_LIBC_LIB)/$(CRT0)
	cp $^ $@

snek-$(BOARD)-install: snek-nano-every-install.in
	$(SNEK_NANO_EVERY_SED) $^ > $@
	chmod +x $@

load: $(HEX) snek-$(BOARD)-install snek-$(BOARD)-reset-port
	./snek-$(BOARD)-install -reset ./snek-$(BOARD)-reset-port -hex $(HEX) load

fuseload: $(HEX) snek-$(BOARD)-install snek-$(BOARD)-reset-port
	./snek-$(BOARD)-install -reset ./snek-$(BOARD)-reset-port -hex $(HEX) fuseload

snek-$(BOARD)-install.1: snek-nano-every-install.1.in
	$(SNEK_NANO_EVERY_SED) $^ > $@

snek-$(BOARD)-reset-port: snek-nano-every-reset-port.c
	gcc -o $@ $^

install: snek-$(BOARD)-install $(HEX) snek-$(BOARD)-install.1
	install -d $(DESTDIR)$(BINDIR)
	install snek-$(BOARD)-install $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(SHAREDIR)
	install snek-$(BOARD)-reset-port $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(HEX) $(DESTDIR)$(SHAREDIR)
	install -d $(DESTDIR)$(MANDIR)/man1
	install -m 0644 snek-$(BOARD)-install.1 $(DESTDIR)$(MANDIR)/man1

clean::
	rm -f *.elf *.hex *.map snek-$(BOARD)-install snek-$(BOARD)-install.1 snek-$(BOARD)-reset-port
