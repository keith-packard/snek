#!/usr/bin/snek
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

import random
import curses
import time

stdscr = 0

snek = []
grow = 0
snak = 0
lines = 25
cols = 40
dx = 0
dy = 0

HEAD = '@'
TAIL = '+'
SNAK = '$'

def _x(p): return p % cols
def _y(p): return p // cols
def _p(x,y): return x + y * cols

def put_snak():
    global snek, snak
    while True:
        snak = _p(random.randrange(cols-2)+1, random.randrange(lines-2)+1)
        for s in snek:
            if snak == s:
                break
        else:
            break
    showp(snak, SNAK)

def showxy(x, y, str):
    stdscr.addstr(y, x, str)

def showp(pos, str):
    showxy(_x(pos), _y(pos), str)

miss = 0
hit_wall = 1
hit_snek = 2
hit_snak = 3

def move_snek():
    global snek, grow
    global dx, dy
    if dx or dy:
        old = snek[len(snek)-1]
        sp = snek[0]
        nx = _x(sp) + dx
        ny = _y(sp) + dy
        if nx < 1 or nx >= cols-1 or ny < 1 or ny >= lines-1:
            return hit_wall
        new = _p(nx, ny)
        if grow > 0:
            tail = snek
            showxy(0,0,"Score %d---" % (len(snek) + 1))
            grow -= 1
        else:
            tail = snek[:-1]
        for t in tail:
            if new == t:
                return hit_snek
        snek = [new] + tail
        showp(old, ' ')
        if tail:
            showp(tail[0], TAIL)
        showp(new, HEAD)
        if new == snak:
            return hit_snak
    return miss

def done(msg):
    stdscr.move(lines-1, 0)
    stdscr.refresh()
    stdscr.nodelay(False)
    curses.nocbreak()
    curses.echo()
    curses.endwin()
    print("You %s. Score %d" % (msg, len(snek)))
    exit(0)

def main():
    global snek, dx, dy, snak, grow
    global stdscr, lines, cols
    stdscr = curses.initscr()
    curses.noecho()
    curses.cbreak()
    stdscr.nodelay(True)
    stdscr.erase()
    random.seed()
    snek = [_p(1,1)]
    put_snak()
    for x in range(1,cols-1):
        showxy(x, 0, '-')
        showxy(x, lines-1, '-')
    for y in range(1,lines-1):
        showxy(0, y, '|')
        showxy(cols-1, y, '|')
    showp(snek[0], HEAD)
    while True:
        stdscr.move(_y(snek[0]), _x(snek[0]))
        stdscr.refresh()
        time.sleep(.1)
        c = stdscr.getch()
        ndx = dx
        ndy = dy
        if c == ord('h'):
            ndx = -1
            ndy = 0
        elif c == ord('j'):
            ndx = 0
            ndy = 1
        elif c == ord('k'):
            ndx = 0
            ndy = -1
        elif c == ord('l'):
            ndx = 1
            ndy = 0
        elif c == ord('q') or c == ord('x'):
            done("quit")
        elif c == ord('p'):
            while stdscr.getch() != ord('p'):
                time.sleep(.1)
        if ndx != -dx and ndy != -dy:
            dx = ndx
            dy = ndy
        hit = move_snek()
        if hit == hit_wall:
            done("hit the wall")
        elif hit == hit_snek:
            done("hit yourself")
        if hit == hit_snak:
            grow = 5
            put_snak()

main()
