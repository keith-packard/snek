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
# Test operator precedence
#

limit = 4

# test boolean operators

for a in (False, True):
    for b in (False, True):
        for c in (False, True):
            # not is higher than and

            assert (not a and b) == ((not a) and b)
            assert (b and not a) == (b and (not a))

            # and is higher than or

            assert (a or b and c) == (a or (b and c))
            assert (a and b or c) == ((a and b) or c)

# test boolean vs comparisons

for a in range(limit):
    for b in range(limit):
        for c in range(limit):
            for d in range(limit):
                # comparison is higher than boolean

                assert (a < b and c < d) == ((a < b) and (c < d))
                assert (a <= b and c <= d) == ((a <= b) and (c <= d))
                assert (a > b and c > d) == ((a > b) and (c > d))
                assert (a >= b and c >= d) == ((a >= b) and (c >= d))
                assert (a != b and c != d) == ((a != b) and (c != d))
                assert (a == b and c == d) == ((a == b) and (c == d))

# test bitwise vs comparisons

for a in range(limit):
    for b in range(limit):
        for c in range(limit):
            for d in range(limit):
                # bitwise is higher than comparison

                assert (a | b < c | d) == ((a | b) < (c | d))

# test bitwise operators

for a in range(limit):
    for b in range(limit):
        for c in range(limit):
            # & is higher than ^
            assert a ^ b & c == a ^ (b & c)
            assert a & b ^ c == (a & b) ^ c

            # ^ is higher than |
            assert a | b ^ c == a | (b ^ c)
            assert a ^ b | c == (a ^ b) | c
