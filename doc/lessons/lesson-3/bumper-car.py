# Bumper Car

power = 1


def MoveOneDir():
    talkto(M2)
    setpower(power)
    setleft()
    on()
    talkto(M3)
    setpower(power)
    setright()
    on()


def MoveOtherDir():
    talkto(M2)
    setpower(power)
    setright()
    on()
    talkto(M3)
    setpower(power)
    setleft()
    on()


def WaitUntilCloseOneDir():
    while read(A7) < 0.3:
        pass


def WaitUntilCloseOtherDir():
    while read(A1) < 0.3:
        pass


def WaitRandom():
    wait = 0.25 + random.randrange(10) / 10
    time.sleep(wait)


def Spin():
    talkto(M2)
    setpower(power)
    setright()
    on()
    talkto(M3)
    setpower(power)
    setright()
    on()


def SpinRandom():
    Spin()
    WaitRandom()


def BumperCar():
    while True:
        WaitUntilCloseOtherDir()

        # Back up a bit and spin around
        MoveOneDir()
        time.sleep(0.5)
        SpinRandom()

        MoveOneDir()
        WaitUntilCloseOneDir()

        # Back up a bit and spin around
        MoveOtherDir()
        time.sleep(0.5)
        SpinRandom()

        MoveOtherDir()


BumperCar()
