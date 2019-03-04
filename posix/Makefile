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

include $(SNEK_ROOT)/snek-install.defs

SNEK_LOCAL_SRC = \
	snek-main.c \
	snek-posix.c \
	snek-curses.c

SNEK_LOCAL_INC = snek-posix.h
SNEK_LOCAL_BUILTINS = snek-posix.builtin

include $(SNEK_ROOT)/snek.defs

OPT=-O0

CFLAGS=-DSNEK_MEM_INCLUDE_NAME $(OPT) -g -I. $(SNEK_CFLAGS) -Werror

LIBS=-lreadline -lm

all: snek snek.desktop

snek: $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SNEK_OBJ) $(LIBS)

install: snek snek.1
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
