#!/usr/bin/python3
#
# Copyright © 2021 Keith Packard <keithp@keithp.com>
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
# Make sure that when nan is compared with any number
# the result is false for any op other than !=
#

nan = float("nan")
inf = float("inf")

for x in (nan, -nan, -inf, inf, -1, 0, 1):
    assert not (nan == x)
    assert not (x == nan)

    assert nan != x
    assert x != nan

    assert not (nan > x)
    assert not (x > nan)

    assert not (nan < x)
    assert not (x < nan)

    assert not (nan >= x)
    assert not (x >= nan)

    assert not (nan <= x)
    assert not (x <= nan)
