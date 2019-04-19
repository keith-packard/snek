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
# Test arithmetic operations
#

if 2 + 3 != 5: exit(1)
if 3 + 2 != 5: exit(1)
if 2 - 3 != -1: exit(1)
if 3 - 2 != +1: exit(1)
if 2 * 3 != 6: exit(1)
if 2 * (3 + 4) != 14: exit(1)
if (2 * 3) + 4 != 10: exit(1)
if 2 * 3 + 4 != 10: exit(1)
if 2 ** 3 ** 4 == 4096: exit(1)
if (2 ** 3) ** 4 != 4096: exit(1)
