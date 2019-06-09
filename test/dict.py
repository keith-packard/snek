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

def fail(which):
    print("fail %s" % which)
    exit(1)

def check(expect,got,which):
    if expect != got:
        fail("%s: %r != %r" % (which, got, expect))

a = { 1:2, 3:4 }
b = { "a":"b", "c":"d" }
c = { 1:2, 1:3 }

check({3:4, 1:2}, a, "a == {3:4, 1:2}")
check({1:3}, c, "c == {1:3}")
check({"c":"d", "a":"b"}, b, 'b = {"c":"d", "a":"b"}')

check(True, 1 in a, "1 in a")
check(False, 2 in a, "2 not in a")
check(True, 3 in a, "3 in a")
check(False, 4 in a, "4 not in a")

check(True, "a" in b, "'a' in b")
check(False, "b" in b, "'b' not in b")
check(True, "c" in b, "'c' in b")
check(False, "d" in b, "'d' not in b")

sum_a_0 = 0
sum_a_1 = 0

for i in a:
    sum_a_0 += i
    sum_a_1 += a[i]

check(4, sum_a_0, "sum a indices == 4")
check(6, sum_a_1, "sum a values == 6")

sum_b_0 = ""
sum_b_1 = ""

for i in b:
    sum_b_0 += i
    sum_b_1 += b[i]

check("ac", sum_b_0, "sum b indices == 'ac'")
check("bd", sum_b_1, "sum b values == 'bd'")

a[1] = 5
b["a"] = "e"

check(5, a[1], "a[1] == 5")
check("e", b["a"], "b['a'] == 'e'")

a[6] = 7
b["f"] = "g"

check(7, a[6], "a[6] == 7")
check("g", b["f"], "b['f'] == 'g'")

del a[1]
del b['c']

check({3:4, 6:7}, a, "a = {3:4, 6:7}")
check({'a': 'e', 'f': 'g'}, b, "b = {'a': 'e', 'f': 'g'}")
