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
# Test scoping for op= expressions.
# In this case, there must be a local declaration of
# the variable before the operation, either a plain assignment
# or a global declaration
#
# a plain assignment will use the local variable
# a global declaration will use the global variable
#

i = 1


def no_decl():
    i += 1
    return i


no_decl()
exit(0)
