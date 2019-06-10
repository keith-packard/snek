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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
#

bright = 0.1

def rainbow(pos):
    if pos < 1:
        r = pos
        g = 1 - r
        b = 0
    elif pos < 2:
        b = pos - 1
        r = 1 - b
        g = 0
    else:
        g = pos - 2
        r = 0
        b = 1 - g
    neopixel(((bright*r,bright*g,bright*b),))

def blink():
    talkto(NEOPIXEL)
    while True:
        for pos in range(0,3,0.01):
            rainbow(pos)
            time.sleep(0.003)

blink()

