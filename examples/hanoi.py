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

import time
import sys

def clear(): print("\x1b[2J", end='')

def display_string(x, y, str): print("\x1b[%d;%dH%s" % (y, x, str), end='')

# A piece for position 'num' is num + 1 + num stars centered in a
# field of max * 2 + 1 characters with spaces on either side. This
# way, every piece is the same number of characters

def bar(n):
    b = ""
    for i in range(n):
        b += "█"
    return b

def make_string(n, c):
    s = ""
    for i in range(n):
        s += c
    return s

def make_piece(num,max):
    return make_string(max - num - 1, ' ') + bar(2*num + 1) + make_string(max - num - 1, ' ')

def make_pieces(max):
    pieces = []
    for n in range(max):
        pieces += [make_piece(n, max)]
    return pieces

towers=[]

# Left edge of the towers

left_x=4

# Position of the bottom of the stacks, set at runtime

bottom_y=0

# delay between moves in seconds

move_delay=0.0250

# Distance between columns on the screen

col_spacing=25

def tower_x(col):
    return col * col_spacing + left_x

# Position of the top of the tower on the screen Shorter
# towers start further down the screen

def tower_y(tower):
    return bottom_y - len(tower)

def clear_above(x, tower):
    display_string(x, tower_y(tower) - 1, "                   ")

# Display a piece from a tower

def display_one(x, y, tower):
    display_string(x, tower_y(tower) + y, tower[y])

# Display a tower

def display_tower(x, tower):
    for y in range(len(tower)):
        display_one(x, y, tower)

# Display all of the towers.

def display_towers(x, towers):
    for i in range(len(towers)):
        display_tower(x + i * col_spacing, towers[i])

def display_flush():
    display_string(1, bottom_y + 1, "")
    sys.stdout.flush()
    time.sleep(move_delay)
    return True

# Display all of the towers, then move the cursor out of the way and
# flush the output

def display_hanoi():
  display_towers(left_x, towers)
  display_flush()

# Reset towers to the starting state, with all of the pieces in the
# first tower and the other two empty

def reset_towers(len):
    global towers
    global bottom_y
    towers = [make_pieces(len), [], []]
    bottom_y = len + 3

def remove_top(tower):
    piece = towers[tower][0]
    new_tower = []
    for i in range(1,len(towers[tower])):
        new_tower += [towers[tower][i]]
    towers[tower] = new_tower
    return piece

def add_top(tower, piece):
    new_tower = [piece]
    for i in range(0, len(towers[tower])):
        new_tower += [towers[tower][i]]
    towers[tower] = new_tower


# Move a piece from the top of one tower to the top of another

def move_piece(frm, to):
    piece = remove_top(frm)
    add_top(to, piece)

# The implementation of the game

def _hanoi(n, frm, to, use):
    if n == 1:
        move_piece(frm, to)
        clear_above(tower_x(frm), towers[frm])
        display_one(tower_x(to), 0, towers[to])
        display_flush()
    else:
        _hanoi(n-1, frm, use, to)
        _hanoi(1, frm, to, use)
        _hanoi(n-1, use, to, frm)

# A pretty interface which
# resets the state of the game,
# clears the screen and runs
# the program

def hanoi(len):
    reset_towers(len)
    clear()
    display_hanoi()
    _hanoi(len, 0, 1, 2)

hanoi(8)
