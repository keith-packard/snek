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

SNEK_NO_BUILD_TARGETS = 1
SNEK_ROOT=..

include $(SNEK_ROOT)/snek-install.defs

all: snekde snekde.desktop

%:
	@echo done $@

install: snekde snekde.desktop snekde.1
	install -d $(DESTDIR)$(BINDIR)
	install snekde $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(APPDIR)
	install snekde.desktop $(DESTDIR)$(APPDIR)
	install -d $(DESTDIR)$(ICONDIR)
	install -m 644 ../snek.svg $(DESTDIR)$(ICONDIR)
	install -d $(DESTDIR)$(MANDIR)/man1
	install snekde.1 $(DESTDIR)$(MANDIR)/man1

snekde.desktop: snekde.desktop.in
	$(SNEK_SED) $^ > $@

snekde: snekde.py
	cp $^ $@
	chmod +x $@

check:
	black --check snekde.py

clean::
	rm -f snekde snekde.desktop
