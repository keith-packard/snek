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

SNEK_LOCAL_SRC = \
	ao-snek.c \
	snek-altos.c \
	snek-eeprom.c \
	snek-math.c \
	snek-gpio.c \
	snek-io.c

SNEK_LOCAL_INC = \
	ao.h \
	ao-adc-samd21.h \
	ao-arch.h \
	ao-arch-funcs.h \
	ao-flash.h \
	ao-led.h \
	ao-notask.h \
	ao-pins.h \
	ao-product.h \
	ao-serial.h \
	ao-snek.h \
	ao-tcc-samd21.h \
	ao-tc-samd21.h \
	ao-usb.h \
	samd21.h

SNEK_LOCAL_BUILTINS = \
	snek-altos.builtin \
	$(SNEK_ROOT)/snek-math.builtin

include $(SNEK_ROOT)/snek-install.defs

CC=$(ARM_CC)

SAMD21_CFLAGS=-mlittle-endian -mcpu=cortex-m0 -mthumb\
	-I$(TOPDIR)/samd21 $(AO_CFLAGS) $(NEWLIB_CFLAGS)

LIBS=-L$(NEWLIB_NANO)/arm-none-eabi/lib/thumb/v6-m -lc -lm -lgcc

WARN_FLAGS=-Wall -Wextra -Werror -Wcast-align \
	-Wpointer-arith \
	-Wstrict-prototypes \
	-Wmissing-prototypes \
	-Wmissing-declarations \
	-Wnested-externs \
	-Wshadow \
	-Warray-bounds=2

OPT=-Os

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

NEWLIB_NANO=/usr/lib/newlib-nano

NEWLIB_CFLAGS= \
	-ffreestanding -nostdlib \
	-isystem $(NEWLIB_NANO)/arm-none-eabi/include

AO_CFLAGS=\
	-std=gnu99 $(WARN_FLAGS) $(OPT) -g

.DEFAULT_GOAL=all

PROGNAME=metro-snek
PROG=$(PROGNAME)-$(SNEK_VERSION).elf
BIN=$(PROGNAME)-$(SNEK_VERSION).bin
UF2=$(PROGNAME)-$(SNEK_VERSION).uf2
MAP=$(PROGNAME)-$(SNEK_VERSION).map

.SUFFIXES: .elf .bin .uf2

.elf.bin:
	$(OBJCOPY) -O binary -j '.text' -j '.ARM.exidx' -j '.rodata' -j '.textram' -j '.data' $^ $@

.bin.uf2:
	python3 ./uf2conv.py -b 0x2000 -c -o $@ $^

#
# AltOS sources
#
ALTOS_SRC = \
	ao-interrupt.c \
	ao-led.c \
	ao-timer.c \
	ao-usb-samd21.c \
	ao-tcc-samd21.c \
	ao-tc-samd21.c \
	ao-adc-samd21.c \
	ao-flash-samd21.c \
	ao-stdio.c \
	ao-notask.c \
	ao-product.c \
	ao-panic.c \
	ao-metro.c

PRODUCT=MetroSnek-v0.0
IDPRODUCT=0x000a

NEWLIB_PRINTF_CFLAGS =

CFLAGS = $(SAMD21_CFLAGS) $(SNEK_CFLAGS) -I.
LDSCRIPT=snek.ld
LDFLAGS=$(CFLAGS) -Wl,-T$(LDSCRIPT)  -n

SRC=$(ALTOS_SRC) $(SNEK_SRC)
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

newt-builtin.h: newt-builtin.py $(BUILTINS)
	python3 newt-builtin.py -o $@ $(BUILTINS)

newtram.h: newt-gram.ll
	lola -o $@ newtram.ll

ao-product.h: ../snek.defs Makefile
	python3 ./make-product.py -v $(SNEK_VERSION) -p MetroSnek > $@

install:
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(UF2) $(DESTDIR)$(SHAREDIR)

uninstall:
