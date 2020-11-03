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

import math

# Make sure lists, tuples and parameter lists can have
# a trailing comma

assert (
    1,
    2,
) == (1, 2)
assert [
    1,
] == [1]
assert [
    1,
    2,
] == [1, 2]

assert {
    1: "hello",
} == {1: "hello"}
assert {
    1: "hello",
    2: "world",
} == {1: "hello", 2: "world"}

assert math.gcd(64, 12) == math.gcd(
    64,
    12,
)
assert (
    math.sin(
        1,
    )
    == math.sin(1)
)
