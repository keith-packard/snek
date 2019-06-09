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

SNEK_LOCAL_SRC = \
	snek-mega.c \
	snek-atmega-eeprom.c \
	snek-io.c \
	snek-atmega-serial.c \
	snek-math.c \
	ef_remainder.c \
	snek-mega-math.c \
	sf_asinh.c \
	ef_acosh.c \
	ef_atanh.c \
	sf_log1p.c \
	sf_log2.c \
	sf_log2_data.c \
	sf_erf.c \
	wf_tgamma.c \
	erf_lgamma.c \
	erf_gamma.c \
	kf_sin.c \
	kf_cos.c

SNEK_LOCAL_VPATH = $(SNEK_ATMEGA)
SNEK_LOCAL_INC = $(SNEK_ATMEGA)/snek-atmega.h machine/ieeefp.h fdlibm.h snek-mega-math.h

SNEK_LOCAL_BUILTINS = \
	snek-mega.builtin \
	snek-math.builtin

include $(SNEK_ROOT)/snek-install.defs

SNEK_CFLAGS = $(SNEK_MOST_WARNINGS) $(SNEK_BASE_CFLAGS)

USBSERIAL_HEX=Arduino-usbserial-atmega16u2-Mega2560-Rev3.hex

BASE=snek-mega-$(SNEK_VERSION)
ELF=$(BASE).elf
HEX=$(BASE).hex
MAP=$(BASE).map
CC=avr-gcc
CFLAGS=-Os -DF_CPU=16000000UL -mmcu=atmega2560 -I. -I$(SNEK_LOCAL_VPATH) -g -fno-jump-tables $(SNEK_CFLAGS) -mcall-prologues -Waddr-space-convert
LDFLAGS=-Wl,-uvfprintf -lprintf_flt -lm \
	-Wl,--defsym -Wl,__TEXT_REGION_LENGTH__=0x3e000 \
	-Wl,--defsym -Wl,__DATA_REGION_LENGTH__=0x1e00 \
	-Wl,--defsym -Wl,__EEPROM_REGION_LENGTH__=0x1000 \
	-Wl,-Map=$(MAP)

PORT=/dev/ttyACM0

all: $(HEX) snek-mega-install

$(HEX): $(ELF)
	avr-objcopy -O ihex -R .eeprom $^ $@

$(ELF): $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@awk '/__data_load_end/{printf("ROM used: %d bytes\n", strtonum($$1)); }' $(MAP)

snek-mega-install: snek-mega-install.in
	$(SNEK_SED) $^ > $@
	chmod +x $@

load: $(HEX) snek-mega-install
	./snek-mega-install -port $(PORT) -hex $(HEX)

install: snek-mega-install $(HEX) snek-mega-install.1
	install -d $(DESTDIR)$(BINDIR)
	install snek-mega-install $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(HEX) $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(USBSERIAL_HEX) $(DESTDIR)$(SHAREDIR)
	install -d $(DESTDIR)$(MANDIR)/man1
	install -m 0644 snek-mega-install.1 $(DESTDIR)$(MANDIR)/man1

clean::
	rm -f *.elf snek*.hex *.map snek-mega-install

load-direct: $(HEX)
	avrdude -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:$(HEX):i 