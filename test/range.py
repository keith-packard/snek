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

expected = 0
times = 0

def start(init, final, interval):
    global expected, times
    times = 0
    expected = (final - init) // interval
    if expected < 0:
        expected = 0

def iter():
    global times
    times += 1

def check(which):
    global times, expected
    if times != expected:
        print("fail %s. times %d expected %d" % (which, times, expected))
        exit(1)

start(0,10,1)
for i in range(10): iter()
check("range(10)")

start(0,10,1)
for i in range(0,10): iter()
check("range(0,10)")

start(0,10,1)
for i in range(0,10,1): iter()
check("range(0,10,1)")

start(1,10,1)
for i in range(1,10): iter()
check("range(1,10)")

start(1,10,1)
for i in range(1,10,1): iter()
check("range(1,10,1)")

start(0,10,-1)
for i in range(0,10,-1): iter()
check("range(0,10,-1)")

start(1,10,-1)
for i in range(1,10,-1): iter()
check("range(1,10,-1)")

start(10,0,1)
for i in range(10,0): iter()
check("range(10,0)")

start(10,0,1)
for i in range(10,0,1): iter()
check("range(10,0,1)")

start(10,0,-1)
for i in range(10,0,-1): iter()
check("range(10,0,-1)")

start(10,1,-1)
for i in range(10,1,-1): iter()
check("range(10,1,-1)")
