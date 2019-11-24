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

SNEK_QEMU = $(SNEK_ROOT)/chips/qemu

ARCH = arm-none-eabi
ARCH_CFLAGS = -mlittle-endian -mcpu=cortex-m3 -mthumb 
PROGNAME = snek-qemu-arm

include $(SNEK_QEMU)/snek-qemu.defs

include $(SNEK_ROOT)/snek-install.defs

SRC=$(SNEK_SRC)
OBJ=$(SRC:.c=.o)

all: $(PROG) snek-arm

$(PROG): Makefile $(OBJ)
	$(call quiet,CC) $(LDFLAGS) -Wl,-Tsnek-qemu-arm.ld -Wl,-M=$(MAP) -o $@ $(OBJ) $(LIBS)

$(OBJ): $(SNEK_INC)

install: $(PROG)
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(PROG) $(DESTDIR)$(SHAREDIR)

snek-arm: snek-arm.in
	$(SNEK_SED) $^ > $@
	chmod +x $@

uninstall:

clean::
	rm -f run-arm
