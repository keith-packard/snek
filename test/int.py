#!/usr/bin/python3
#
# Copyright © 2019 Paulo Henrique Silva <ph.silva@gmail.com>
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


def fail(which):
    print("fail %s" % which)
    exit(1)


def check(a, b, which):
    if a > b or a < b:
        fail(which)


check(1_2_3, 123, "1_2_3 should be equal to 123")
check(10_000_000, 10000000, "10_000_000 should be equal to 10000000")
