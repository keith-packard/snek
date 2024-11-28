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

SNEK_ROOT=../../..

SNEK_LOCAL_SRC = \
	snek-main.c \
	snek-math.c \
	snek-random.c \
	snek-input.c \
	snek-esp32.c \
	snek-io.c

SNEK_LOCAL_INC = \
	$(SNEK_EXT_INC)

SNEK_LOCAL_BUILTINS = \
	snek-esp32.builtin \
	snek-math.builtin \
	snek-random.builtin \
	snek-input.builtin

include $(SNEK_ROOT)/snek.defs

CMakeLists.txt: $(SNEK_SRC)
	(echo "idf_component_register(SRCS"; \
	for i in $^; do echo '"'$$i'"'; done; \
	echo 'INCLUDE_DIRS "../../.." ".." ".")') > $@

all: snek-gram.h snek-builtin.h CMakeLists.txt

clean::
	rm -f CMakeLists.txt
