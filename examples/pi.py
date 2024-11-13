#!/usr/bin/snek
#
# Imagine a circle inscribed in a square. The ratio of their areas is
# the same as the ratio of a random point in the square also being in
# the circle
#
# We'll use a circle of radius 1, which means the square is 2x2. The
# circle has area π, the square has area 4:
#
# circle / square = π / 4 = inside / tries
#
# π = inside / tries * 4
#

import random
import time


def pi(outer):
    start = time.monotonic()
    inner = 100
    inside = 0
    for o in range(outer):
        for i in range(inner):
            # Pick a random point in the square. This point will have
            # coordinates from (-1,-1) to (1,1)
            x = random.random() * 2 - 1
            y = random.random() * 2 - 1
            # Check to see if the point is inside the circle
            if x * x + y * y <= 1:
                inside += 1
    print("inside %d pi %g" % (inside, inside / (outer * inner) * 4))
    stop = time.monotonic()
    print("time %f" % (stop - start))


pi(100)
