
LightSensor = A1
TapeThreshold = 0.45

def OnTape():
    return(read(LightSensor) > TapeThreshold)

def LineBug():
    while True:
        while OnTape():
            StartLeftTurn()

        while not OnTape():
            StartRightTurn()
