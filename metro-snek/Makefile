#
# AltOS build
#
#

LDFLAGS=$(CFLAGS) -L$(TOPDIR)/samd21 -Wl,-T$(LDSCRIPT)  -n

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

SNEK_LOCAL_VPATH=$(AO_VPATH):../snek-bits

.SUFFIXES: .elf .bin .uf2

.elf.bin:
	$(OBJCOPY) -O binary -j '.text' -j '.ARM.exidx' -j '.rodata' -j '.textram' -j '.data' $^ $@

.bin.uf2:
	python3 ./uf2conv.py -b 0x2000 -c -o $@ $^

SNEK_LOCAL_INC = \
	ao.h \
	ao_arch.h \
	ao_arch_funcs.h \
	ao_pins.h \
	samd21.h

SNEK_LOCAL_SRC = \
	ao_snek.c \
	snek_altos.c \
	snek_eeprom.c \
	snek_gpio.c \
	snek_io.c

SNEK_LOCAL_BUILTINS = \
	snek-altos.builtin

SNEK_ROOT = $(HOME)/src/snek

include $(SNEK_ROOT)/snek.defs

#
# Common AltOS sources
#
ALTOS_SRC = \
	ao_interrupt.c \
	ao_led.c \
	ao_timer.c \
	ao_serial_samd21.c \
	ao_usb_samd21.c \
	ao_tcc_samd21.c \
	ao_tc_samd21.c \
	ao_adc_samd21.c \
	ao_flash_samd21.c \
	ao_stdio.c \
	ao_notask.c \
	ao_product.c \
	ao_panic.c \
	ao_metro.c

PRODUCT=MetroSnek-v0.0
IDPRODUCT=0x000a

NEWLIB_PRINTF_CFLAGS =

CFLAGS = $(SAMD21_CFLAGS) $(SNEK_CFLAGS) -I.
LDSCRIPT=snek.ld

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
	rm -f *.o *.elf *.bin *.uf2 *.map

newt-builtin.h: newt-builtin.py $(BUILTINS)
	python3 newt-builtin.py -o $@ $(BUILTINS)

newtram.h: newt-gram.ll
	lola -o $@ newtram.ll

install:

uninstall:
