#
# Copyright (c) 1980, 1993
#       The Regents of the University of California.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

#
# snake - crt hack game.
#
# You move around the screen with arrow keys trying to pick up money
# without getting eaten by the snake.  hjkl work as in vi in place of
# arrow keys.  You can leave at the exit any time.
#

import time
import sys
import random
import curses
import math


def cashvalue():
    global loot
    global penalty
    return chunk * (loot - penalty) / 25


def _x(p):
    return p % 256 - 128


def _y(p):
    return p // 256 - 128


def _p(x, y):
    return (x + 128) + (y + 128) * 256


PENALTY = 10  # % penalty for invoking spacewarp

ME = "I"
SNAKEHEAD = "S"
SNAKETAIL = "s"
TREASURE = "$"
GOAL = "#"
LINES = 0
COLS = 0


def MIN(a, b):
    if a < b:
        return a
    return b


stdscr = 0


def erase():
    stdscr.erase()


def refresh():
    stdscr.refresh()


def getch():
    return stdscr.getch()


def move(x, y):
    stdscr.move(x, y)


def mvaddstr(y, x, str):
    stdscr.addstr(y, x, str)


def pchar(s, str):
    stdscr.addstr(_y(s) + 1, _x(s) + 1, str)
    refresh()


def endwin():
    curses.nocbreak()
    curses.echo()
    curses.endwin()


def delay(t):
    time.sleep(t * 0.05)


you = _p(0, 0)
money = _p(0, 0)
finish = _p(0, 0)
snake = [_p(0, 0)] * 6

loot = 0
penalty = 0
moves = 0
fast = 1

lcnt = 25
ccnt = 80

chunk = 0


def error(s):
    print(s)
    exit(1)


def main():
    global penalty, loot, finish, you, money, snake, chunk
    random.seed(time.monotonic())
    penalty = 0
    loot = 0
    initscr()
    i = MIN(lcnt, ccnt)
    if i < 4:
        error("screen too small for a fair game.")
    #
    #  chunk is the amount of money the user gets for each $.
    #  The formula below tries to be fair for various screen sizes.
    #  We only pay attention to the smaller of the 2 edges, since
    #  that seems to be the bottleneck.
    #  This formula is a hyperbola which includes the following points:
    #   (24, $25)       (original scoring algorithm)
    #   (12, $40)       (experimentally derived by the "feel")
    #   (48, $15)       (a guess)
    #  This will give a 4x4 screen $99/shot.  We don't allow anything
    #  smaller than 4x4 because there is a 3x3 game where you can win
    #  an infinite amount of money.
    #
    if i < 12:
        i = 12  # otherwise it isn't fair
    #
    #  Compensate for border.  This really changes the game since
    #  the screen is two squares smaller but we want the default
    #  to be $25, and the high scores on small screens were a bit
    #  much anyway.
    # /
    i += 2
    chunk = (675.0 / (i + 6)) + 2.5  #  min screen edge
    finish = snrand()
    you = snrand()
    money = snrand()
    snake[0] = snrand()
    for i in range(1, 6):
        if i < 6:
            snake[i] = chase(snake[i - 1])
    setup()
    mainloop()
    exit(0)


# Main command loop
def mainloop():
    global moves, money, loot, you
    repeat = 1
    lastc = 0
    while True:
        # Highlight you, not left & above
        move(_y(you) + 1, _x(you) + 1)
        refresh()
        c = getch()
        if c <= ord("9") and c >= ord("0"):
            repeat = c - ord("0")
            while True:
                c = getch()
                if c < ord("0") or ord("9") < c:
                    break
                repeat = 10 * repeat + ord(c) - ord("0")
        elif c != ord("."):
            repeat = 1
        if c == ord("."):
            c = lastc
        lastc = c
        if c == ord("x"):
            endwin()
            length(moves)
            exit(0)
        elif c == ord("p") or c == ord("d"):
            snap()
            continue
        elif c == ord("w"):
            spacewarp(0)
            continue
        elif c == ord("A"):
            repeat = _x(you)
            c = ord("h")
        elif c == ord("H") or c == ord("S"):
            repeat = _x(you) - _x(money)
            c = ord("h")
        elif c == ord("T"):
            repeat = _y(you)
            c = ord("k")
        elif c == ord("K") or c == ord("E"):
            repeat = _y(you) - _y(money)
            c = ord("k")
        elif c == ord("P"):
            repeat = ccnt - 1 - _y(you)
            c = ord("l")
        elif c == ord("L") or c == ord("F"):
            repeat = _x(money) - _x(you)
            c = ord("l")
        elif c == ord("B"):
            repeat = lcnt - 1 - _y(you)
            c = ord("j")
        elif c == ord("J") or c == ord("C"):
            repeat = _y(money) - _y(you)
            c = ord("j")
        for k in range(1, repeat + 1):
            moves += 1
            if c == ord("s") or c == ord("h") or c == ord("\b"):
                if _x(you) > 0:
                    pchar(you, " ")
                    you = _p(_x(you) - 1, _y(you))
                    pchar(you, ME)
            elif c == ord("f") or c == ord("l") or c == ord(" "):
                if _x(you) < ccnt - 1:
                    pchar(you, " ")
                    you = _p(_x(you) + 1, _y(you))
                    pchar(you, ME)
            elif c == ord("e") or c == ord("k") or c == ord("i"):
                if _y(you) > 0:
                    pchar(you, " ")
                    you = _p(_x(you), _y(you) - 1)
                    pchar(you, ME)
            elif c == ord("c") or c == ord("j") or c == ord("\n") or c == ord("m"):
                if _y(you) + 1 < lcnt:
                    pchar(you, " ")
                    you = _p(_x(you), _y(you) + 1)
                    pchar(you, ME)
            if you == money:
                loot += 25
                if k < repeat:
                    pchar(you, " ")
                while True:
                    money = snrand()
                    if money == finish:
                        continue
                    if money == you:
                        continue
                    if _x(money) < 5 and _y(money) == 0:
                        continue
                    break
                pchar(money, TREASURE)
                winnings(cashvalue())
                continue
            if you == finish:
                win(finish)
                endwin()
                print("You have won with $%d." % cashvalue())
                length(moves)
                exit(0)
            if pushsnake():
                print("pushsnake true")
                break


def initscr():
    global stdscr, lcnt, ccnt
    stdscr = curses.initscr()
    curses.noecho()
    curses.cbreak()
    COLS = 80
    LINES = 25
    lcnt = LINES - 2
    ccnt = COLS - 2
    erase()


#
# setup the board
#


def setup():
    pchar(you, ME)
    pchar(finish, GOAL)
    pchar(money, TREASURE)
    for i in range(1, 6):
        pchar(snake[i], SNAKETAIL)
    pchar(snake[0], SNAKEHEAD)
    drawbox()


def drawbox():
    for i in range(1, ccnt + 1):
        mvaddstr(0, i, "-")
        mvaddstr(lcnt + 1, i, "-")
    for i in range(0, lcnt + 2):
        mvaddstr(i, 0, "|")
        mvaddstr(i, ccnt + 1, "|")


def snrand():
    while True:
        p = _p(random.randrange(ccnt), random.randrange(lcnt))
        # make sure it's not on top of something else
        if _y(p) == 0 and _x(p) < 5:
            continue
        if p == you:
            continue
        if p == money:
            continue
        if p == finish:
            continue
        for i in range(6):
            if p == snake[i]:
                break
        else:
            break
    return p


dirs = ("N ", "NE", "E ", "SE", "S ", "SW", "W ", "NW")
mx = (0, 1, 1, 1, 0, -1, -1, -1)
my = (-1, -1, 0, 1, 1, 1, 0, -1)
absv = (1, 1.4, 1, 1.4, 1, 1.4, 1, 1.4)
oldw = 0


def chase(sp):
    global oldw, loot
    # this algorithm has bugs; otherwise the snake would get too good
    d = _p(_x(you) - _x(sp), _y(you) - _y(sp))
    v1 = math.sqrt(_x(d) ** 2 + _y(d) ** 2)
    w = 0
    max = 0
    wt = [0] * 8
    for i in range(8):
        vp = _x(d) * mx[i] + _y(d) * my[i]
        v2 = absv[i]
        if v1 > 0:
            vp = vp / (v1 * v2)
        else:
            vp = 1
        if vp > max:
            max = vp
            w = i
    for i in range(8):
        d = _p(_x(sp) + mx[i], _y(sp) + my[i])
        wt[i] = 0
        if _x(d) < 0 or _x(d) >= ccnt or _y(d) < 0 or _y(d) >= lcnt:
            continue
        #
        # Change to allow snake to eat you if you're on the money,
        # otherwise, you can just crouch there until the snake goes
        # away.  Not positive it's right.
        #
        # if d[1] == 0 and d[0] < 5: continue
        #
        if d == money:
            continue
        if d == finish:
            continue
        if i == w:
            wt[i] = loot // 10
        else:
            wt[i] = 1
        if i == oldw:
            wt[i] += loot // 20
    w = 0
    for i in range(8):
        w += wt[i]
    vp = random.randrange(w)
    vpo = vp
    for i in range(8):
        if vp < wt[i]:
            break
        else:
            vp -= wt[i]
    else:
        printw("failure\n")
        i = 0
    while wt[i] == 0:
        i += 1
    refresh()
    w = i
    oldw = w
    return _p(_x(sp) + mx[w], _y(sp) + my[w])


def spacewarp(w):
    global loot
    you = snrand()
    p = _p(COLS / 2 - 8, LINES / 2 - 1)
    if _x(p) < 0:
        p = _p(0, _y(p))
    if _y(p) < 0:
        p = _p(_x(p), 0)
    if w:
        str = "BONUS!!!"
        loot = loot - penalty
        penalty = 0
    else:
        str = "SPACE WARP!!!"
        penalty += loot / PENALTY
    for j in range(3):
        erase()
        refresh()
        delay(5)
        pchar(_p(_x(p) + 1, _y(p) + 1), str)
        refresh()
        delay(10)
    setup()
    winnings(cashvalue())


def snap():
    if not stretch(money):
        if not stretch(finish):
            pchar(you, "?")
            refresh()
            delay(10)
            pchar(you, ME)
    refresh()


def abs(x):
    if x < 0:
        return -x
    return x


def stretch(ps):
    p = you
    if abs(_x(ps) - _x(you)) < (ccnt / 12) and (_y(you) != _y(ps)):
        if _y(you) < _y(ps):
            for l in range(_y(you) + 1, _y(ps) + 1):
                p = _p(_x(p), l)
                pchar(p, "v")
            refresh()
            delay(10)
            while _y(p) > _y(you):
                chk(p)
                p = _p(_x(p), _y(p) - 1)
        else:
            p = _p(_x(p), _y(you) - 1)
            while _y(p) >= _y(ps):
                pchar(p, "^")
                p = _p(_x(p), _y(p) - 1)
            refresh()
            delay(10)
            while _y(p) < _y(you):
                chk(p)
                p = _p(_x(p), _y(p) + 1)
        return 1
    else:
        if abs(_y(ps) - _y(you)) < lcnt / 7 and _x(you) != _x(ps):
            p = _p(_x(p), _y(you))
            if _x(you) < _x(ps):
                p = _p(_x(you) + 1, _y(p))
                while _x(p) <= _x(ps):
                    pchar(p, ">")
                    p = _p(_x(p) + 1, _y(p))
                refresh()
                delay(10)
                while _x(p) > _x(you):
                    chk(p)
                    p = _p(_x(p) - 1, _y(p))
            else:
                p = _p(_x(you) - 1, _y(p))
                while _x(p) >= _x(ps):
                    pchar(p, "<")
                    p = _p(_x(p) - 1, _y(p))
                refresh()
                delay(10)
                while _x(p) < _x(you):
                    chk(p)
                    p = _p(_x(p) + 1, _y(p))
            return 1
    return 0


def surround(ps):
    if _x(ps) == 0:
        ps = _p(_x(ps) + 1, _y(ps))
    if _y(ps) == 0:
        ps = _p(_x(ps), _y(ps) + 1)
    if _y(ps) == LINES - 1:
        ps = _p(_x(ps), _y(ps) - 1)
    if _x(ps) == COLS - 1:
        ps = _p(_x(ps) - 1, _y(ps))
    mvaddstr(_y(ps), _x(ps), "/*\\")
    mvaddstr(_y(ps) + 1, _x(ps), "* *")
    mvaddstr(_y(ps) + 2, _x(ps), "\\*/")
    for j in range(20):
        pchar(ps, "@")
        refresh()
        delay(1)
        pchar(ps, " ")
        refresh()
        delay(1)
    mvaddstr(_y(ps), _x(ps), "   ")
    mvaddstr(_y(ps) + 1, _x(ps), "o.o")
    mvaddstr(_y(ps) + 2, _x(ps), "\\_/")
    refresh()
    delay(6)
    mvaddstr(_y(ps), _x(ps), "   ")
    mvaddstr(_y(ps) + 1, _x(ps), "o.-")
    mvaddstr(_y(ps) + 2, _x(ps), "\\_/")
    refresh()
    delay(6)
    mvaddstr(_y(ps), _x(ps), "   ")
    mvaddstr(_y(ps) + 1, _x(ps), "o.o")
    mvaddstr(_y(ps) + 2, _x(ps), "\\_/")
    refresh()
    delay(6)


def win(ps):
    boxsize = 10
    x = ps
    for j in range(1, boxsize):
        for k in range(j):
            pchar(x, "#")
            x = _p(_x(x), _y(x) - 1)
        for k in range(j):
            pchar(x, "#")
            x = _p(_x(x) + 1, _y(x))
        j += 1
        for k in range(j):
            pchar(x, "#")
            x = _p(_x(x), _y(x) + 1)
        for k in range(j):
            pchar(x, "#")
            x = _p(_x(x) - 1, _y(x))
        refresh()
        delay(1)


def pushsnake():
    global loot
    #
    # My manual says times doesn't return a value.  Furthermore, the
    # snake should get his turn every time no matter if the user is
    # on a fast terminal with typematic keys or not.
    # So I have taken the call to times out.
    #
    issame = 0
    for i in range(4, -1, -1):
        if snake[i] == snake[5]:
            issame += 1
    if issame == 0:
        pchar(snake[5], " ")
    # Need the following to catch you if you step on the snake's tail
    tmp = snake[5]
    for i in range(4, -1, -1):
        snake[i + 1] = snake[i]
    snake[0] = chase(snake[1])
    pchar(snake[1], SNAKETAIL)
    pchar(snake[0], SNAKEHEAD)
    for i in range(6):
        if snake[i] == you or tmp == you:
            surround(you)
            i = (cashvalue()) % 10
            bonus = random.randrange(10)
            mvaddstr(lcnt + 1, 0, "%d" % bonus)
            refresh()
            delay(30)
            if bonus == i:
                spacewarp(1)
                return 1
            endwin()
            if loot >= penalty:
                print("\nYou and your $%d have been eaten\n" % cashvalue())
            else:
                print("\nThe snake ate you.  You owe $%d.\n" % -cashvalue())
            length(moves)
            exit(0)
    return 0


def chk(sp):
    if sp == money:
        pchar(sp, TREASURE)
        return 2
    if sp == finish:
        pchar(sp, GOAL)
        return 3
    if sp == snake[0]:
        pchar(sp, SNAKEHEAD)
        return 4
    for j in range(1, 6):
        if sp == snake[j]:
            pchar(sp, SNAKETAIL)
            return 4
    if _x(sp) < 4 and _y(sp) == 0:
        winnings(cashvalue())
    if _y(you) == 0 and _x(you) < 4:
        pchar(you, ME)
        return 5
    if sp == you:
        pchar(sp, ME)
        return 1
    pchar(sp, " ")
    return 0


def winnings(won):
    if won > 0:
        pchar(_p(0, 0), "$%d" % won)


def length(num):
    print("You made %d moves.\n" % num)


main()
