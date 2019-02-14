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

VERSION = 1.0

SNEK_NO_BUILD_TARGETS = 1
SNEK_ROOT = .
include snek.defs

PREFIX=$(DESTDIR)/usr/local
LIBDIR = $(PREFIX)/lib
SNEKLIB = $(LIBDIR)/snek
PKGCONFIG = $(LIBDIR)/pkgconfig

SUBDIRS = posix snek-duino

all:
	+for dir in $(SUBDIRS); do (cd $$dir && make); done

%:
	+for dir in $(SUBDIRS); do (cd $$dir && make $@); done

check: all
	+cd test && make $@

LIBFILES = \
	snek.defs \
	$(SNEK_SRC) \
	$(SNEK_RAW_INC) \
	$(SNEK_BUILTINS) \
	$(SNEK_ROOT)/snek-gram.ll \
	$(SNEK_ROOT)/snek-builtin.py

PKGFILES = \
	snek.pc

install: $(LIBFILES) $(PKGFILES) install-snekde install-posix
	mkdir -p $(SNEKLIB)
	cp -a $(LIBFILES) $(SNEKLIB)
	mkdir -p $(PKGCONFIG)
	cp -a $(PKGFILES) $(PKGCONFIG)

install-snekde:
	+@cd snekde && make install

install-posix:
	+@cd posix && make install

snek.pc: snek.pc.in
	sed -e 's;@SNEKLIB@;$(SNEKLIB);' -e 's/@VERSION@/$(VERSION)/' $^ > $@
