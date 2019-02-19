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
SNEK_LOCAL_SRC = \
	snek-duino.c \
	snek-duino-eeprom.c \
	snek-duino-serial.c

SNEK_LOCAL_INC = snek-duino.h
SNEK_LOCAL_BUILTINS = snek-duino.builtin

all: snek-duino.hex

include ../snek.defs

CC=avr-gcc
CFLAGS=-Os -DF_CPU=16000000UL -mmcu=atmega328p -I. -g -fno-jump-tables $(SNEK_CFLAGS)
LDFLAGS=-Wl,-uvfprintf -lprintf_flt -lm \
	-Wl,--defsym -Wl,__TEXT_REGION_LENGTH__=0x8000 \
	-Wl,--defsym -Wl,__DATA_REGION_LENGTH__=0x7a0 \
	-Wl,--defsym -Wl,__EEPROM_REGION_LENGTH__=0x400 \
	-Wl,-Map=snek-duino.map

ISP = usbtiny
#ISP = avrisp2

#
# lfuse: 0xff
#	CKDIV8:		1	no divide by 8
#	CKOUT:		1	no clock output
#	SUT1:		1	crystal osc, slow startup
#	SUT0:		1	crystal osc, slow startup
#	CKSEL3:		1	low swing osc, 8-16MHz
#	CKSEL2:		1	low swing osc, 8-16MHz
#	CKSEL1:		1	low swing osc, 8-16MHz
#	CKSEL0:		1	crystal osc
# hfuse:
#	RSTDISBL:	1	reset enabled
#	DWEN:		1	debug wire disabled
#	SPIEN:		0	SPI program enabled
#	WDTON:		1	watch dog disabled
#	EESAVE:		0	EEPROM saved across chip erase
#	BOOTSZ1:	1	256 words
#	BOOTSZ0:	1	256 words
#	BOOTRST:	1	boot to address 0
# efuse:
#	BODLEVEL2	1	brown out 2.5-2.9V
#	BODLEVEL1	0	brown out 2.5-2.9V
#	BODLEVEL0	1	brown out 2.5-2.9V

set-fuse:
	avrdude -F -V -c $(ISP) -p ATMEGA328P -U lfuse:w:0xff:m
	avrdude -F -V -c $(ISP) -p ATMEGA328P -U hfuse:w:0xd7:m
	avrdude -F -V -c $(ISP) -p ATMEGA328P -U efuse:w:0xfd:m

load: snek-duino.hex
	avrdude -F -V -c $(ISP) -p ATMEGA328P -U flash:w:snek-duino.hex

snek-duino.hex: snek-duino
	avr-objcopy -O ihex -R .eeprom snek-duino $@

snek-duino: $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	awk '/__data_load_end/{printf("ROM used: %d bytes\n", strtonum($$1)); }' snek-duino.map

clean::
	rm -f snek-duino snek-duino.hex
