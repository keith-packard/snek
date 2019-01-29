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
#
# tuples don't have in-place operation,
# so += makes a new tuple
#

a = ()
b = a
a += (1,2)

if a != (1,2): exit(1)
if b != (): exit(1)

#
# lists have in-place operation,
# so += does not make a new list
# while a + b does make a new list
#

a = []
b = a
c = a + [1,2]
a += [1,2]

if a != [1,2]: exit(1)
if b != [1,2]: exit(1)
if c != [1,2]: exit(1)

a[0] = 3

if a != [3,2]: exit(1)
if b != [3,2]: exit(1)
if c != [1,2]: exit(1)
