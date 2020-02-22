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
# Check short-circuiting and/or
#

or00 = (0 or 0) + 0
or10 = (1 or 0) + 0
or01 = (0 or 1) + 0
or11 = (1 or 1) + 0

and00 = (0 and 0) + 0
and10 = (1 and 0) + 0
and01 = (0 and 1) + 0
and11 = (1 and 1) + 0

if or00 != False:
    exit(1)
if or10 != True:
    exit(1)
if or01 != True:
    exit(1)
if or11 != True:
    exit(1)

if and00 != False:
    exit(1)
if and10 != False:
    exit(1)
if and01 != False:
    exit(1)
if and11 != True:
    exit(1)
