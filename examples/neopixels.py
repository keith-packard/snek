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
#
# Rotate a rainbow through a set of pixels
#

DOWN = BUTTONA
UP = BUTTONB

num_pixels = 10
bright = 0.05
pixels = [0] * num_pixels
for i in range(num_pixels):
    pixels[i] = [0, 0, 0]


def wheel(i, pos):
    global bright
    if pos < 0 or pos > 255:
        r = 0
        g = 0
        b = 0
    elif pos < 85:
        r = pos / 85
        g = 1 - r
        b = 0
    elif pos < 170:
        b = (pos - 85) / 85
        r = 1 - b
        g = 0
    else:
        g = (pos - 170) / 85
        r = 0
        b = 1 - g
    pixels[i][0] = r * bright
    pixels[i][1] = g * bright
    pixels[i][2] = b * bright


def rainbow_cycle(wait):
    global bright
    for j in range(255):
        for i in range(num_pixels):
            pixel_index = (i * 256 // num_pixels) + j
            wheel(i, pixel_index & 255)
        neopixel(pixels)
        if read(DOWN):
            if bright > 0.02:
                bright -= 0.01
                print(bright)
        elif read(UP):
            if bright < 0.99:
                bright += 0.01
                print(bright)
        time.sleep(wait)


def cycles():
    talkto(NEOPIXEL)
    while True:
        rainbow_cycle(0.001)


cycles()
