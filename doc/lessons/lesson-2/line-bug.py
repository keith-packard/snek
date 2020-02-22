# Line Bug program

LightSensor = A1
TapeThreshold = 0.45


def OnTape():
    return read(LightSensor) > TapeThreshold


RightWheel = M1
LeftWheel = M3
Speed = 1


def StartLeftTurn():
    talkto(LeftWheel)
    off()
    talkto(RightWheel)
    setleft()
    setpower(Speed)
    on()


def StartRightTurn():
    talkto(RightWheel)
    off()
    talkto(LeftWheel)
    setright()
    setpower(Speed)
    on()


def LineBug():
    while True:
        while OnTape():
            StartLeftTurn()

        while not OnTape():
            StartRightTurn()


LineBug()
