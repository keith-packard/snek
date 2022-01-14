#
# Copyright © 2020 Keith Packard <keithp@keithp.com>
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
# Test chaining comparison operators
#

assert 0 < 1 < 2
assert 0 < 1 <= 2
assert not (0 < 1 == 2)
assert 0 < 1 != 2
assert not (0 < 1 >= 2)
assert not (0 < 1 > 2)
assert 0 <= 1 < 2
assert 0 <= 1 <= 2
assert not (0 <= 1 == 2)
assert 0 <= 1 != 2
assert not (0 <= 1 >= 2)
assert not (0 <= 1 > 2)
assert not (0 == 1 < 2)
assert not (0 == 1 <= 2)
assert not (0 == 1 == 2)
assert not (0 == 1 != 2)
assert not (0 == 1 >= 2)
assert not (0 == 1 > 2)
assert 0 != 1 < 2
assert 0 != 1 <= 2
assert not (0 != 1 == 2)
assert 0 != 1 != 2
assert not (0 != 1 >= 2)
assert not (0 != 1 > 2)
assert not (0 >= 1 < 2)
assert not (0 >= 1 <= 2)
assert not (0 >= 1 == 2)
assert not (0 >= 1 != 2)
assert not (0 >= 1 >= 2)
assert not (0 >= 1 > 2)
assert not (0 > 1 < 2)
assert not (0 > 1 <= 2)
assert not (0 > 1 == 2)
assert not (0 > 1 != 2)
assert not (0 > 1 >= 2)
assert not (0 > 1 > 2)
assert "a" < "b" < "c"
assert "a" < "b" <= "c"
assert not ("a" < "b" == "c")
assert "a" < "b" != "c"
assert not ("a" < "b" >= "c")
assert not ("a" < "b" > "c")
assert "a" <= "b" < "c"
assert "a" <= "b" <= "c"
assert not ("a" <= "b" == "c")
assert "a" <= "b" != "c"
assert not ("a" <= "b" >= "c")
assert not ("a" <= "b" > "c")
assert not ("a" == "b" < "c")
assert not ("a" == "b" <= "c")
assert not ("a" == "b" == "c")
assert not ("a" == "b" != "c")
assert not ("a" == "b" >= "c")
assert not ("a" == "b" > "c")
assert "a" != "b" < "c"
assert "a" != "b" <= "c"
assert not ("a" != "b" == "c")
assert "a" != "b" != "c"
assert not ("a" != "b" >= "c")
assert not ("a" != "b" > "c")
assert not ("a" >= "b" < "c")
assert not ("a" >= "b" <= "c")
assert not ("a" >= "b" == "c")
assert not ("a" >= "b" != "c")
assert not ("a" >= "b" >= "c")
assert not ("a" >= "b" > "c")
assert not ("a" > "b" < "c")
assert not ("a" > "b" <= "c")
assert not ("a" > "b" == "c")
assert not ("a" > "b" != "c")
assert not ("a" > "b" >= "c")
assert not ("a" > "b" > "c")


def add(a, b):
    return a + b


assert add(1 == 2 != 0, 3) == 3
assert add(1 == 1 != 1, 4) == 4
