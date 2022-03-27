#
# Copyright © 2022 Keith Packard <keithp@keithp.com>
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
# Test string operations
#

abcde = "abcde"
vwxyz = "vwxyz"

# array operator
assert abcde[1] == "b"
assert abcde[-1] == "e"

# in operator
assert "bc" in abcde
assert "be" not in abcde
assert not "be" in abcde

# plus
assert abcde + vwxyz == "abcdevwxyz"
assert vwxyz + abcde == "vwxyzabcde"

# times
assert abcde * 3 == "abcdeabcdeabcde"
assert 3 * vwxyz == "vwxyzvwxyzvwxyz"
