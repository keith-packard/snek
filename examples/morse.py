#
# Morse code example
#

from time import *

T = 1.2 / 12

a = (
    ".-",
    "-...",
    "-.-.",
    "-..",
    ".",
    "..-.",
    "--.",
    "....",
    "..",
    ".---",
    "-.-",
    ".-..",
    "--",
    "-.",
    "---",
    ".--.",
    "--.-",
    ".-.",
    "...",
    "-",
    "..-",
    "...-",
    ".--",
    "-..-",
    "-.--",
    "--..",
)
n = (
    "-----",
    ".----",
    "..---",
    "...--",
    "....-",
    ".....",
    "-....",
    "--...",
    "---..",
    "----.",
)


def d(l):
    l = ord(l)
    if l < 58:
        return n[l - 48]
    return a[l - 97]


def l(c):
    for e in d(c):
        if e == ".":
            onfor(T)
        else:
            onfor(3 * T)
        sleep(T)
    sleep(3 * T)


def m(t):
    for c in t:
        if c == " ":
            sleep(6 * T)
        else:
            l(c)


def cq():
    talkto(LED)
    while True:
        m("cq cq de k7wq  ")


cq()
