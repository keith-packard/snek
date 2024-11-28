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

PROGNAME=snek-hifive1revb
PRODUCT_NAME=snek-hifive1revb

# Note -- must set FREEDOM_METAL before building

FREEDOM_METAL ?= /must/set/FREEDOM_METAL

FREEDOM_METAL_PICO = $(FREEDOM_METAL)/pico
FREEDOM_METAL_GLOSS = $(FREEDOM_METAL)/gloss
LD_SCRIPT = metal.default.lds
FREEDOM_METAL_SRC = $(FREEDOM_METAL)/src
FREEDOM_METAL_DRIVERS = $(FREEDOM_METAL_SRC)/drivers
FREEDOM_METAL_INC = $(FREEDOM_METAL)

SNEK_LOCAL_VPATH = $(SNEK_FE310):$(SNEK_AO):$(FREEDOM_METAL_SRC):$(FREEDOM_METAL_DRIVERS):$(FREEDOM_METAL_PICO):$(FREEDOM_METAL_GLOSS)

PICOLIBC_PRINTF_CFLAGS = -DPICOLIBC_FLOAT_PRINTF_SCANF

PICOLIBC_CFLAGS= \
	-specs=picolibc.specs \
	-ffunction-sections \
	-fdata-sections \
	$(PICOLIBC_PRINTF_CFLAGS)

OPT=-Os -g

WARN_FLAGS=-Wall -Wextra -Wcast-align \
	-Wpointer-arith \
	-Wstrict-prototypes \
	-Wmissing-prototypes \
	-Wmissing-declarations \
	-Wnested-externs \
	-Wshadow \
	-Warray-bounds=2\
	-Wno-missing-prototypes\
	-Wno-unused-parameter\
	-Wno-strict-prototypes\
	-Wno-missing-declarations\
	-Wno-cast-align\
	-Wno-maybe-uninitialized

AO_CFLAGS=\
	-std=c18 $(WARN_FLAGS) $(OPT) -g

HIFIVE1REVB_CFLAGS=-march=rv32imac -mabi=ilp32 \
	-I. -I$(SNEK_ROOT) -I$(FREEDOM_METAL_INC) $(PICOLIBC_CFLAGS)

CFLAGS = $(HIFIVE1REVB_CFLAGS) $(SNEK_CFLAGS) $(AO_CFLAGS)

LDFLAGS=$(SNEK_LDFLAGS) $(CFLAGS) -n

LIBS=-lm

ARCH=riscv64-unknown-elf
CC=$(ARCH)-gcc
OBJCOPY=$(ARCH)-objcopy

PROG=$(PROGNAME)-$(SNEK_VERSION).elf
HEX=$(PROGNAME)-$(SNEK_VERSION).hex
MAP=$(PROGNAME)-$(SNEK_VERSION).map

.SUFFIXES: .elf .bin

.elf.bin:
	$(OBJCOPY) -O binary -S $^ $@

SNEK_LOCAL_SRC = \
	vector.S \
	entry.S \
	init.c \
	inline.c \
	fixed-clock.c \
	riscv_plic0.c \
	riscv_clint0.c \
	riscv_cpu.c \
	sifive_fe310-g000_prci.c \
	sifive_fe310-g000_hfrosc.c \
	sifive_fe310-g000_hfxosc.c \
	sifive_fe310-g000_lfrosc.c \
	sifive_fe310-g000_pll.c \
	sifive_uart0.c \
	sifive_spi0.c \
	sifive_gpio-leds.c \
	sifive_wdog0.c \
	sifive_rtc0.c \
	sifive_gpio0.c \
	sifive_i2c0.c \
	i2c.c \
	sifive_local-external-interrupts0.c \
	sys_exit.c \
	button.c \
	cache.c \
	clock.c \
	cpu.c \
	gpio.c \
	interrupt.c \
	led.c \
	lock.c \
	memory.c \
	pmp.c \
	privilege.c \
	rtc.c \
	shutdown.c \
	spi.c \
	switch.c \
	synchronize_harts.c \
	time.c \
	timer.c \
	trap.S \
	uart.c \
	watchdog.c \
	snek-metal.c \
	snek-math.c \
	snek-random.c \
	snek-io.c \
	snek-input.c \
	snek-metal-gpio.c \
	snek-metal-uart.c \
	snek-metal-builtin.c

SNEK_LOCAL_INC = \
	$(SNEK_FE310_INC)

SNEK_LOCAL_BUILTINS = \
	snek-math.builtin \
	snek-random.builtin \
	snek-gpio.builtin \
	snek-input.builtin \
	snek-hifive1revb.builtin

include $(SNEK_ROOT)/snek-install.defs

SRC=$(SNEK_SRC)
OBJ=$(patsubst %.S,%.o,$(patsubst %.c,%.o,$(SRC)))

echo::
	echo $(OBJ)

all: $(PROG) $(HEX)

$(PROG): Makefile $(OBJ)
	$(call quiet,CC) $(LDFLAGS) -T$(LD_SCRIPT) -Wl,-M=$(MAP) -o $@ $(OBJ) $(LIBS)

$(HEX): $(PROG)
	$(OBJCOPY) -O ihex -S $^ $@

$(OBJ): $(SNEK_INC) $(FREEDOM_METAL)

distclean:	clean

clean::
	rm -f *.o $(PROGNAME)*.elf $(PROGNAME)*.hex $(PROGNAME)*.map ao-product.h
	rm -f snek-board-install

snek-board-install: snek-board-install.in
	$(SNEK_SED) $^ > $@
	chmod +x $@

echo::
	echo PROG is $(PROG)

install: $(PROG)
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(PROG) $(DESTDIR)$(SHAREDIR)

uninstall:

$(FREEDOM_METAL):
	if [ ! -d $@ ]; then \
		echo "Need to set FREEDOM_METAL before building snek-hifive1revb";\
		exit 1;\
	fi
