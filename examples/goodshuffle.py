#!/usr/bin/python3

from random import *
from time import *

seed(monotonic())

# "Shuffle" a list, but *wrong*.

narray = 16
a = [0] * narray
h = [0] * narray

for _ in range(100000):
    for i in range(narray):
        a[i] = i
    for i in range(narray):
        j = i + randrange(narray - i)
        tmp = a[j]
        a[j] = a[i]
        a[i] = tmp
    for i in range(narray):
        if a[i] == narray - 1:
            h[i] += 1
            break

print(h)
