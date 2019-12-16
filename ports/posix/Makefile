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

SNEK_ROOT=../..

SNEK_LOCAL_SRC = \
	snek-main.c \
	snek-posix.c \
	snek-math.c \
	snek-curses.c \
	snek-input.c

SNEK_LOCAL_INC = snek-posix.h
SNEK_LOCAL_CFLAGS = 
SNEK_LOCAL_BUILTINS = snek-posix.builtin $(SNEK_ROOT)/snek-math.builtin $(SNEK_ROOT)/snek-input.builtin

include $(SNEK_ROOT)/snek-install.defs

OPT?=-O3

CFLAGS+=-DSNEK_MEM_INCLUDE_NAME $(OPT) -g -I. $(SNEK_CFLAGS) -Werror $(CPPFLAGS) $(CFLAGS_POSIX)

LIBS=-lreadline -lm

all: snek snek.desktop

snek: $(SNEK_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SNEK_OBJ) $(LIBS)

install: snek snek.desktop snek.1
	install -d $(DESTDIR)$(BINDIR)
	install snek $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(APPDIR)
	install snek.desktop $(DESTDIR)$(APPDIR)
	install -d $(DESTDIR)$(MANDIR)/man1
	install snek.1 $(DESTDIR)$(MANDIR)/man1

snek.desktop: snek.desktop.in
	$(SNEK_SED) $^ > $@

clean::
	rm -f snek snek.desktop
