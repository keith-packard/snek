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

tuple = (1, 2, 3, 4, 5)
list = [6, 7, 8, 9, 10]
string = "abcde"

#
# Test fetching from ordered objects
#

for i in range(1, 5):
    assert tuple[i - 1] == i
    assert tuple[-i] == 6 - i

    assert list[i - 1] == 5 + i
    assert list[-i] == 11 - i

    assert string[i - 1] == chr(ord("a") + i - 1)
    assert string[-i] == chr(ord("f") - i)
