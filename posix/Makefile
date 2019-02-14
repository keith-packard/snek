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

SNEK_ROOT=..

PREFIX=$(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin

SNEK_LOCAL_SRC = \
	snek-main.c \
	snek-posix.c \
	snek-curses.c

SNEK_LOCAL_INC = snek-posix.h
SNEK_LOCAL_BUILTINS = snek-posix.builtin

all: snek

include $(SNEK_ROOT)/snek.defs

OPT=-O0

CFLAGS=-DSNEK_MEM_INCLUDE_NAME $(OPT) -g -I. $(SNEK_CFLAGS)

LIBS=-lreadline -lm

snek: $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SNEK_OBJ) $(LIBS)

install: snek
	install $^ $(BINDIR)

clean::
	rm -f snek
