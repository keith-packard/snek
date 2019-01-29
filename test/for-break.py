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
#!/usr/bin/python3

def test():
    val = 5
    tries = 0
    while tries < 10:
        tries += 1
        for x in range(10):
            if x == val:
                val = 20
                break
        else:
            break
    return tries

if test() != 2: exit(1)
