servo = M4
motor = M1
front = A1
back = A2


def see(sensor):
    return read(sensor) > 0.1


def setservo(v):
    talkto(servo)
    if v < 0:
        setleft()
        v = -v
    else:
        setright()
    setpower(v)
    on()


def go(t, d):
    talkto(motor)
    s = front
    if d:
        setright()
        s = front
    else:
        setleft()
        s = back
    setpower(1)
    onfor(t)


def straight(t, d):
    setservo(0)
    go(t, d)


def left(t, d):
    setservo(1)
    go(t, d)


def right(t, d):
    setservo(-1)
    go(t, d)


def bounce():
    straight(random.randrange(10) / 10 + 1, True)
    right(random.randrange(10) / 10 + 1, False)
    left(random.randrange(10) / 10 + 1, True)
    straight(random.randrange(10) / 10 + 1, False)


while True:
    bounce()
