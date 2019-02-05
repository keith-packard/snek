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

SNEK_LOCAL_SRC = snek-main.c snek-posix.c
SNEK_LOCAL_INC = snek-posix.h
SNEK_LOCAL_BUILTINS = snek-posix.builtin

all: snek

include $(SNEK_ROOT)/snek.defs

CFLAGS=-DSNEK_MEM_INCLUDE_NAME -O0 -g -I. $(SNEK_CFLAGS)

snek: $(SNEK_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SNEK_OBJ) -lm

clean::
	rm -f snek
