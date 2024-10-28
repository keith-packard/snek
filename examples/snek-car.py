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

from time import *

# This program is for the Crickit FeatherWing board

# Motor 1 is on the right side of the car
mr = (MOTOR1A, MOTOR1B)

# Motor 2 is on the left side of the car
ml = (MOTOR2A, MOTOR2B)

# SIGNAL1 is wired to a Phototransistor in the front of the car
pf = SIGNAL1

# SIGNAL2 is wired to a Phototransistor in the rear of the car
pr = SIGNAL2

# Pick some speeds
f_speed = 0.5
r_speed = 0.5
t_speed_f = 0.6
t_speed_s = 0.2


# Move straight forward
def forw():
    talkto(mr)
    setpower(f_speed)
    setright()
    on()
    talkto(ml)
    setpower(f_speed)
    setleft()
    on()


# Move straight back
def back():
    talkto(mr)
    setpower(r_speed)
    setleft()
    on()
    talkto(ml)
    setpower(r_speed)
    setright()
    on()


# Rotate left
def left():
    talkto(mr)
    setpower(t_speed_f)
    setright()
    on()
    talkto(ml)
    setpower(t_speed_f)
    setright()
    on()


# Rotate right
def right():
    talkto(mr)
    setpower(t_speed_f)
    setleft()
    on()
    talkto(ml)
    setpower(t_speed_f)
    setleft()
    on()


# Stop
def stop():
    talkto(ml)
    setpower(0)
    off()
    talkto(mr)
    setpower(0)
    off()


# Move forward until we approach something
def go_forw():
    forw()
    while read(pf) < 0.25:
        pass
    stop()


# Move backwards until we approach something
def go_back():
    back()
    while read(pr) < 0.25:
        pass
    stop()


# Hook the functions together in a simple
# way to demonstrate how they work
def bumper():
    while True:
        go_forw()
        back()
        sleep(0.5)
        stop()
        left()
        sleep(0.25)
        stop()
        go_forw()
        go_back()
        go_forw()


bumper()
