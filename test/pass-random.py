#!/usr/bin/python3
#
# Copyright © 2020 Keith Packard <keithp@keithp.com>
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

import random

def check(got, r):
    if got < 0 or r <= got:
        print("random value %d outside range %d" % (got, r))
        exit(1)

for r in (2, 163844, 100000, 8388607):
    for t in range(100):
        v = random.randrange(r)
        check(v, r)
