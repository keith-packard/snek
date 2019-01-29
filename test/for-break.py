#!/usr/bin/python3

def test():
    val = 5
    tries = 0
    while tries < 10:
        tries += 1
        for x in range(10):
            if x == val:
                val = 20
                break
        else:
            break
    return tries

if test() != 2: exit(1)
