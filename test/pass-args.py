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
def check(want, got, mess):
    if got != want:
        print("fail: %s (want %r got %r)" % (mess, want, got))
        exit(1)

def req(x): return x
def default(x=0): return x

def req_req(x,y): return x-y
def req_default(x,y=42): return x-y
def default_default(x=63,y=42): return x-y

def many_args(x,y,z,a=7,b=11,c=13): return x * y * z * a * b * c

check(1,req(1),"pos req")
check(2,req(x=2),"named req")
check(3,default(3),"pos default")
check(4,default(x=4),"named default")

check(5,req_req(6,1), "pos_pos req_req")
check(6,req_req(8,y=2), "pos_named req_req")
check(7,req_req(x=10,y=3), "named1_named2 req_req")
check(8,req_req(y=4,x=12), "named2_named1 req_req")

check(9,req_default(13,4), "pos_pos req_default")
check(10,req_default(14,y=4), "pos_named req_default")
check(11,req_default(x=16,y=5), "named1_named2 req_default")
check(12,req_default(y=6,x=18), "named2_named1 req_default")
check(13,req_default(55), "pos_default req_default")
check(14,req_default(x=56), "named1_default req_default")

check(15,default_default(22,7), "pos_pos default_default")
check(16,default_default(24,y=8), "pos_named default_default")
check(17,default_default(x=26,y=9), "named1_named2 default_default")
check(18,default_default(y=10,x=28), "named2_named1 default_default")
check(19,default_default(61), "pos_default default_default")
check(20,default_default(x=62), "named1_default default_default")
check(21,default_default(), "default_default default_default")

check(2*3*5*7*11*13,many_args(2,3,5), "many_args ()")
check(2*3*5*17*11*13,many_args(2,3,5,a=17), "many_args (a=17)")
check(2*3*5*17*19*13,many_args(2,3,5,a=17,b=19), "many_args (a=17,b=19)")
check(2*3*5*17*19*23,many_args(2,3,5,a=17,b=19,c=23), "many_args (a=17,b=19,c=23)")
