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

from random import *

servo = M4
motor = M1
front = A1
back = A2


def see(sensor):
    return read(sensor) > 0.1


def setservo(v):
    talkto(servo)
    if v < 0:
        setleft()
        v = -v
    else:
        setright()
    setpower(v)
    on()


def go(t, d):
    talkto(motor)
    s = front
    if d:
        setright()
        s = front
    else:
        setleft()
        s = back
    setpower(1)
    onfor(t)


def straight(t, d):
    setservo(0)
    go(t, d)


def left(t, d):
    setservo(1)
    go(t, d)


def right(t, d):
    setservo(-1)
    go(t, d)


def bounce():
    straight(randrange(10) / 10 + 1, True)
    right(randrange(10) / 10 + 1, False)
    left(randrange(10) / 10 + 1, True)
    straight(randrange(10) / 10 + 1, False)


while True:
    bounce()
