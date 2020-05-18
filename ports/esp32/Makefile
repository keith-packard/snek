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
include ../../snek-install.defs

ESP32_IDF ?= ./esp32-idf

TARGET=snek-esp32-$(SNEK_VERSION).bin
TARGET_DASH=snek-esp32-$(SNEK_VERSION_DASH).bin

all: $(TARGET)

$(TARGET): build/$(TARGET_DASH)
	cp $^ $@

build/$(TARGET_DASH):
	make -C main all
	SNEK_VERSION_DASH=$(SNEK_VERSION_DASH) $(ESP32_IDF) build

clean::
	$(ESP32_IDF) clean
	+make -C main clean

load:
	$(ESP32_IDF) -p /dev/ttyUSB1 flash

connect:
	minicom -D /dev/ttyUSB1 -b 115200

install: $(TARGET)
	install -d $(DESTDIR)$(SHAREDIR)
	install -m 0644 $(TARGET) $(DESTDIR)$(SHAREDIR)
