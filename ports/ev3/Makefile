#
# Copyright © 2020 Mikhail Gusarov <dottedmag@dottedmag.net>
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

vpath snek-posix.c ../posix

SNEK_ROOT=../..

CC=arm-linux-gnueabi-gcc
PROGNAME=snek-ev3
PROG=$(PROGNAME)-$(SNEK_VERSION)

SNEK_LOCAL_SRC = \
	snek-main.c \
	snek-posix.c \
	snek-math.c \
	snek-input.c \
	sensors.c \
	motors.c \
	utils.c

SNEK_LOCAL_INC = snek-ev3.h utils.h sensors.h motors.h
SNEK_LOCAL_CFLAGS = -DSNEK_USE_GLIBC_2_4_MATH
SNEK_LOCAL_BUILTINS = \
	snek-ev3.builtin \
	$(SNEK_ROOT)/snek-math.builtin \
	$(SNEK_ROOT)/snek-input.builtin

include $(SNEK_ROOT)/snek-install.defs

SNEK_CFLAGS = $(SNEK_MOST_WARNINGS) $(SNEK_BASE_CFLAGS)

OPT?=-O3

CFLAGS+=-DSNEK_MEM_INCLUDE_NAME $(OPT) -g -I. $(SNEK_CFLAGS) -Werror $(CPPFLAGS)

LIBS=-lm

all: $(PROG)

$(PROG): $(SNEK_OBJ)
	$(call quiet,CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SNEK_OBJ) $(LIBS)

install: $(PROG)
	install -d $(DESTDIR)$(SHAREDIR)
	install $(PROG) $(DESTDIR)$(SHAREDIR)

clean::
	rm -f $(PROG)
