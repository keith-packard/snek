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

# so many options to pick here; just do a few simple tests

assert "%d" % 3 == "3"
assert "%d %%" % 3 == "3 %"
assert "%d" % 3.1 == "3"
assert "%f" % 3.1 == "3.100000"
assert "%g" % 3.1 == "3.1"
assert "%e" % 3.1 == "3.100000e+00"
assert "%s" % "hello" == "hello"
assert "%r" % "hello" == "'hello'"
assert ("%d" + "x" * 300 + "%d") % (
    12,
    34,
) == "12xxxxxxxxxx" + "xx" * 140 + "xxxxxxxxxx34"
assert "%r" % {1: "hello", 2: "world"} == "{1: 'hello', 2: 'world'}"
