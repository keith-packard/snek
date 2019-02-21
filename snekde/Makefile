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

include ../snek-install.defs

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

snekde.desktop: snekde.desktop.in ALWAYS
	sed -e 's;@ICONDIR@;$(ICONDIR);g' -e 's;@BINDIR@;$(BINDIR);g' snekde.desktop.in > snekde.desktop

ALWAYS:

snekde: snekde.py
	cp $^ $@
	chmod +x $@

clean:
	rm -f snekde snekde.desktop
