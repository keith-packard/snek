#!/usr/bin/python3
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

def fail(which):
    print("fail %s" % which)
    exit(1)

def check(a,b,which):
    if a > b or a < b:
        fail(which)

# 1.701412e38 is 0x7f000001. Make sure both of these values look like
# numbers

check (-1.701412e38, 2 * -8.50706e37, "-1.701412e38")
check ( 1.701412e38, 2 *  8.50706e37, " 1.701412e38")

# in 32-bit floats, these should be +/- inf

check (2 * -1.701412e38, 4 * -8.50706e37, "2 * -1.701412e38")
check (2 *  1.701412e38, 4 *  8.50706e37, "2 *  1.701412e38")
