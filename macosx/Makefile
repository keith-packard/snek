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

MACOSX_RES=16 32 128 256 512
MACOSX_ICON_PNGS = $(shell for i in $(MACOSX_RES); do echo snek-$$i.png; done)
MACOSX_ICON = snek.icns
MACOSX_DIST = snek-mac-$(SNEK_VERSION).dmg

SNEKDE = ../snekde/snekde.py
SNEKDE_SCRIPTS = runsnekde snekde-script
SNEKDE_FILES = $(SNEKDE_SCRIPTS) snekde-Info.plist $(SNEKDE)

SNEK = snek
SNEK_SCRIPTS =  runsnek snek-script
SNEK_FILES = $(SNEK_SCRIPTS) snek-Info.plist $(SNEK)

MACOSX_INSTALL = install-macosx

MACOSX_DOCS = ../doc/snek.pdf ../COPYING ReadMe-Mac.rtf

MACOSX_FILES=$(SNEKDE_FILES) $(SNEK_FILES) serial $(MACOSX_ICON) $(MACOSX_SCRIPTS) $(MACOSX_INSTALL) $(MACOSX_DOCS) ../examples/*.py

all: $(MACOSX_DIST)

$(MACOSX_ICON): ../snek.svg
	for i in $(MACOSX_RES); do rsvg-convert -w $$i -h $$i -o snek-$$i.png $^; done
	png2icns $@ $(MACOSX_ICON_PNGS)

$(MACOSX_DIST): $(MACOSX_FILES)
	rm -f $@
	rm -rf macosx
	mkdir -p  macosx/examples
	cp -a $(MACOSX_DOCS) macosx
	cp -a $(MACOSX_INSTALL) macosx
	cp -a $(FIRMWARE) macosx
	cp -a ../examples/* macosx/examples
	#
	# snekde
	#
	mkdir -p macosx/snekde.app/Contents/Resources macosx/snekde.app/Contents/MacOS
	echo 'APPLSN.O' > macosx/snekde.app/Contents/PkgInfo
	cp -p snekde-Info.plist macosx/snekde.app/Contents/Info.plist
	cp -p $(MACOSX_ICON) macosx/snekde.app/Contents/Resources
	cp -a $(SNEKDE) macosx/snekde.app/Contents/Resources
	cp -a serial macosx/snekde.app/Contents/Resources
	cp -p $(SNEKDE_SCRIPTS) macosx/snekde.app/Contents/MacOS/
	#
	# snek
	#
	mkdir -p macosx/snek.app/Contents/Resources macosx/snek.app/Contents/MacOS
	echo 'APPLSN.O' > macosx/snek.app/Contents/PkgInfo
	cp -p snek-Info.plist macosx/snek.app/Contents/Info.plist
	cp -p $(MACOSX_ICON) macosx/snek.app/Contents/Resources
	cp -a $(SNEK) macosx/snek.app/Contents/Resources
	cp -p $(SNEK_SCRIPTS) macosx/snek.app/Contents/MacOS/
	#
	# generate iso image
	#
	genisoimage -D -V Snek-$(SNEK_VERSION) -no-pad -r -apple -o $@ macosx

serial: always
	rsync -a --delete --delete-excluded --exclude '__pycache__' --exclude '*.pyc' /usr/lib/python3/dist-packages/serial/ serial

always:

snekde-Info.plist: snekde-Info.plist.in
	$(SNEK_SED) $^ > $@

snek-Info.plist: snek-Info.plist.in
	$(SNEK_SED) $^ > $@

$(SNEK): FORCE
	./build-mac $(SNEKMAC) $@

FORCE:

install:
	echo install done

upload: $(MACOSX_DIST)
	rsync -avz $(MACOSX_DIST) $(SNEKDIST)

clean:
	rm -rf serial
	rm -rf $(SNEK)
	rm -f $(MACOSX_ICON) $(MACOSX_ICON_PNGS)
	rm -f snekde-Info.plist snek-Info.plist
	rm -rf macosx *.dmg

