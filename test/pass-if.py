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
def g(i):
    if i == 0:
        return 100
    elif i == 1:
        return 101
    elif i == 2:
        return 102
    else:
        return 103


if not (
    g(0) == 100 and g(1) == 101 and g(2) == 102 and g(3) == 103 and g("hello") == 103
):
    exit(1)
