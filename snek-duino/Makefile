#
# Copyright © 2019 Keith Packard <keithp@keithp.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#

SNEK_ROOT = ..
SNEK_LOCAL_SRC = snek-duino.c
SNEK_LOCAL_INC = snek-duino.h
SNEK_LOCAL_BUILTINS = snek-duino.builtin

all: snek-duino.hex

include ../snek.defs

CC=avr-gcc
CFLAGS=-Os -DF_CPU=16000000UL -mmcu=atmega328p -I. -g -fno-jump-tables $(SNEK_CFLAGS)
LDFLAGS=-Wl,-uvfprintf -lprintf_flt -lm \
	-Wl,--defsym -Wl,__TEXT_REGION_LENGTH__=0x8000 \
	-Wl,--defsym -Wl,__DATA_REGION_LENGTH__=0x7c0 \
	-Wl,--defsym -Wl,__EEPROM_REGION_LENGTH__=0x400 \
	-Wl,-Map=snek-duino.map

load: snek-duino.hex
	avrdude -F -V -c usbtiny -p ATMEGA328P -U flash:w:snek-duino.hex

snek-duino.hex: snek-duino
	avr-objcopy -O ihex -R .eeprom snek-duino $@

snek-duino: $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean::
	rm -f snek-duino snek-duino.hex
