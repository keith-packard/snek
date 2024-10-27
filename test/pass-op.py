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
#
# Test arithmetic operations
#

from math import *

if 2 + 3 != 5:
    exit(1)
if 3 + 2 != 5:
    exit(1)
if 2 - 3 != -1:
    exit(1)
if 3 - 2 != +1:
    exit(1)
if 2 * 3 != 6:
    exit(1)
if 2 * (3 + 4) != 14:
    exit(1)
if (2 * 3) + 4 != 10:
    exit(1)
if 2 * 3 + 4 != 10:
    exit(1)
if 2**3**4 == 4096:
    exit(1)
if (2**3) ** 4 != 4096:
    exit(1)
if 7.8 // 2.2 != 3.0:
    exit(1)
if 8.7 // 2.2 != 3.0:
    exit(1)
if 8.8 // 2.2 != 4.0:
    exit(1)
if -6.7 // 2.2 != -4.0:
    exit(1)
if -6.6 // 2.2 != -3.0:
    exit(1)
if not isclose(7.8 % 2.2, 1.2):
    exit(1)
if not isclose(-7.8 % 2.2, 1.0):
    exit(1)
inf = float("inf")

vals = (-inf, -2, -1, 0, 1, 2, inf)

for first in range(0, len(vals)):
    assert vals[first] == vals[first] + 0

    for second in range(first, len(vals)):
        assert vals[first] <= vals[second] + 0

        assert vals[second] >= vals[first] + 0

    for second in range(first + 1, len(vals)):
        assert vals[first] < vals[second] + 0

        assert vals[second] > vals[first] + 0

    for second in range(0, len(vals)):
        if second != first:
            assert vals[first] != vals[second] + 0
