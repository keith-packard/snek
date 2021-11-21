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

tests = {
    "[1, 2, 3]": [1, 2, 3],
    "('a', 'b', 'c')": ("a", "b", "c"),
    "{1: 'hello', 2: 'world'}": {1: "hello", 2: "world"},
    "hello world": "hello world",
    "1": "1",
    "0.5": 1 / 2,
}

for expect in tests:
    value = tests[expect]
    result = str(value)
    assert result == expect
