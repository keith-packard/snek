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

SNEK_LOCAL_SRC = \
	snek-windows.c

SNEK_LOCAL_INC = snek-windows.h
SNEK_LOCAL_BUILTINS = snek-windows.builtin

WINDOWS_FILES=snek.exe snek.ico snekde.py

include $(SNEK_ROOT)/snek-install.defs

WINDOWS_INSTALL=build/nsis/snek-windows-$(SNEK_VERSION_DASH).exe

CC=i686-w64-mingw32-gcc
MINGFLAGS=-Wall -Wextra -mconsole

OPT=-O0

CFLAGS=$(OPT) -g -I. $(SNEK_CFLAGS) $(MINGFLAGS)

LIBS=-lm

all: $(WINDOWS_INSTALL)

$(WINDOWS_INSTALL): $(WINDOWS_FILES) snek.cfg
	pynsist snek.cfg

snek.cfg: snek.cfg.in
	$(SNEK_SED) $^ > $@

snek.exe: $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SNEK_OBJ) $(LIBS)

vpath %.svg ..

snekde.py: ../snekde/snekde.py
	cp $^ $@

.SUFFIXES: .svg .build .icns .ico .rc .o .exe

.svg.build:
	for i in $(RES); do rsvg-convert -w $$i -h $$i -o $*-$$i.png $^; done && touch $@

.build.ico:
	icotool -c -o $@ $(shell for i in $(RES); do echo $*-$$i.png; done)

.ico.rc:
	./make-rc "$*" $(VERSION) > $@

RES=16 24 32 48 64 72 96 128 180 256

install:
	echo install done

upload: $(WINDOWS_INSTALL)
	scp $(WINDOWS_INSTALL) $(SNEKDIST)

clean::
	rm -f snek.exe snek.ico snekde.py snek.cfg
	rm -rf build

