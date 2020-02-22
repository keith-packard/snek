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

# Foil the python3 compiler into creating separate tuples
def twople(a, b):
    return (a, b)


def cat(a, b):
    a + b


al = [1, 2]
adl = [[1, 2], [3, 4]]
bl = [1, 2]
bdl = [[1, 2], [3, 4]]
at = twople(1, 2)
adt = (twople(1, 2), twople(3, 4))
bt = twople(1, 2)
bdt = (twople(1, 2), twople(3, 4))
astr = cat("h", "ello")
bstr = cat("he", "llo")


def fail(which):
    print("fail %s" % which)
    exit(1)


# == tests elements of lists and tuples and strings
if not (al == bl):
    fail("al == bl")
if not (adl == bdl):
    fail("adl == bdl")
if not (at == bt):
    fail("at == bt")
if not (adt == bdt):
    fail("adt == bdt")
if not (astr == bstr):
    fail("astr == bstr")

# is tests for same lists/tuples, but matching string contents
if al is bl:
    fail("al is bl")
if adl is bdl:
    fail("adl is bdl")
if at is bt:
    fail("at is bt")
if adt is bdt:
    fail("adt is bdt")
if not (astr is bstr):
    fail("astr is bstr")

# in uses ==, not is

if not (al in adl):
    exit(1)
if not (bl in bdl):
    exit(1)
if not (at in adt):
    exit(1)
if not (bt in bdt):
    exit(1)
