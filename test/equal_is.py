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

# == tests elements of both lists and tuples
if not ([1,2] == [1,2]): exit (1)
if not ((1,2) == (1,2)): exit (1)

if not ([[1,2],[3,4]] == [[1,2],[3,4]]): exit (1)
if not (((1,2),(3,4)) == ((1,2),(3,4))): exit (1)

# is tests elements of tuples but only container of lists
if     ([1,2] is [1,2]): exit (1)
if not ((1,2) is (1,2)): exit (1)

if     ([[1,2],[3,4]] is [[1,2],[3,4]]): exit (1)
if not (((1,2),(3,4)) is ((1,2),(3,4))): exit (1)

# in tests using == not is

if not ([1,2] in [[1,2],[3,4]]): exit(1)
if not ((1,2) in ((1,2),(3,4))): exit(1)
