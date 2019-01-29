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
vpath %.c .:..
vpath %.h .:..

INC = \
	../snek.h \
	snek-builtin.h \
	snek-duino.h \
	snek-gram.h

SNEK_SRC = \
	snek-builtin.c \
	snek-code.c \
	snek-error.c \
	snek-for.c \
	snek-frame.c \
	snek-func.c \
	snek-list.c \
	snek-lex.c \
	snek-memory.c \
	snek-name.c \
	snek-parse.c \
	snek-poly.c \
	snek-print.c \
	snek-string.c

SRC = \
	snek-duino.c $(SNEK_SRC)

OBJ = $(SRC:.c=.o)

CC=avr-gcc
CFLAGS=-Os -DF_CPU=16000000UL -mmcu=atmega328p -I. -I.. -g -fno-jump-tables
LDFLAGS=-Wl,-uvfprintf -lprintf_flt -lm \
	-Wl,--defsym -Wl,__TEXT_REGION_LENGTH__=0x8000 \
	-Wl,--defsym -Wl,__DATA_REGION_LENGTH__=0x7c0 \
	-Wl,--defsym -Wl,__EEPROM_REGION_LENGTH__=0x400 \
	-Wl,-Map=snek-duino.map

all: snek-duino.hex

load: snek-duino.hex
	avrdude -F -V -c usbtiny -p ATMEGA328P -U flash:w:snek-duino.hex

snek-duino.hex: snek-duino
	avr-objcopy -O ihex -R .eeprom snek-duino $@

snek-duino: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

BUILTINS = \
	../snek-keyword.builtin \
	../snek-base.builtin \
	snek-duino.builtin

snek-builtin.h: ../snek-builtin.py $(BUILTINS)
	python3 ../snek-builtin.py -o $@ $(BUILTINS)

snek-gram.h: ../snek-gram.ll
	lola -o $@ ../snek-gram.ll

$(OBJ): $(INC)

clean:
	rm -f $(OBJ) snek-gram.h snek-builtin.h snek-duino snek-duino.hex
