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

ops = ("<", "<=", "==", "!=", ">=", ">")
n_vals = (0, 1, 2)
s_vals = ("a", "b", "c")


def perm(vals, ops):
    for f_op in ops:
        for s_op in ops:
            print("if %r %s %r %s %r:" % (vals[0], f_op, vals[1], s_op, vals[2]))
            print(
                ' print("assert %r %s %r %s %r")'
                % (vals[0], f_op, vals[1], s_op, vals[2])
            )
            print("else:")
            print(
                ' print("assert not(%r %s %r %s %r)")'
                % (vals[0], f_op, vals[1], s_op, vals[2])
            )


perm(n_vals, ops)
perm(s_vals, ops)
