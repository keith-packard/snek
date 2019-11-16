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
    if l < 58: return n[l-48]
    return a[l-97]

def l(c):
    for e in d(c):
        if e == '.':
            onfor(T)
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
    talkto(D13)
    while True:
        m("cq cq de kd7sqg  ")

cq()
