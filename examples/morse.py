#
# Morse code example
#

T = 1.2 / 12

a=('.-', '-...', '-.-.', '-..', '.', '..-.', '--.',
   '....', '..', '.---', '-.-', '.-..', '--', '-.',
   '---', '.--.', '--.-', '.-.', '...', '-', '..-',
   '...-', '.--', '-..-', '-.--', '--..')
n=('-----', '.----', '..---', '...--', '....-',
   '.....', '-....', '--...', '---..', '----.')

def d(l):
    l = ord(l)
    if 97 <= l and l <= 122:
        return a[l-97]
    return n[l-48]

def l(c):
    es = d(c)
    for e in es:
        if e == '.':
            onfor(T)
            time.sleep(T)
        else:
            onfor(3*T)
            time.sleep(T)
    time.sleep(3*T)

def m(t):
    for c in t:
        if c == ' ':
            time.sleep(6*T)
        else:
            l(c)

def cq():
    talkto(13)
    while True:
        m("cq cq de kd7sqg  ")

cq()
