#
# PID line bug
#

# This needs to be determined experimentally and
# should be above the value of 'black'
black = 0.77
white = 0.06
threshold = (black + white) / 2


# Provide an 'error' value between -1 and 1 indicating
# how far off we are from the line
def measure():
    error = -1

    # Start from the rightmost sensor and work leftwards
    # to determine where the edge appears.
    for s in range(A1, A8 + 1):
        v = read(s)
        if v > threshold:
            # Adjust the error value for 'how black' the
            # sensor sees
            return error + (black - v) / 2
        error += 0.25
    return error


# Experimentally determined PID constants
Kp = 1.1
Ki = 0.004
Kd = 1

integral = 0
previous_error = 0


# Compute control value based on measured error
def process():
    global integral, previous_error

    # Measure the current error
    error = measure()

    # Integrate error into the I term
    integral += error

    # Take the difference into the D term
    derivative = error - previous_error

    # Save the current error for use next time
    previous_error = error

    # Return the computed PID output, combining
    # the current error, integral and derivative terms
    return Kp * error + Ki * integral + Kd * derivative


# Set motor speed and direction using
# values from -1 to 1
def set_speed(m, val):
    talkto(m)
    if val < 0:
        setleft()
        setpower(-val)
    else:
        setright()
        setpower(val)


#
# Set speeds of both motors using  values from -2 to 2.
#
# Values from -2 to -1 turn left motor full forward
# and right motor from full reverse to stop
#
# Values from -1 to 0 turn left motor full forward
# and right motor from stop to full forward
#
# Values from 0 to 1 turn right motor full forward
# and left motor from full forward to stop
#
# Values from 1 to 2 turn right motor full forward
# and left motor from stop to full reverse
def set_speeds(value):
    if value < 0:
        # reduce speed of left motor
        set_speed(M3, 1 + value)
        set_speed(M1, -1)
    else:
        # reduce speed of right motor
        set_speed(M1, value - 1)
        set_speed(M3, 1)


# Initialize
def init():
    global integral, previous_error, Ki_run

    # Turn both motors on, but set the power to zero
    talkto(M1)
    setpower(0)
    on()
    talkto(M3)
    setpower(0)
    on()

    # Initialize PID loop values
    integral = 0
    previous_error = 0
    Ki_run = 0


# compute control value, update motor speeds
def loop():
    init()
    while True:
        # Compute desired motor setting
        value = process()

        # Set the motors
        set_speeds(value)


loop()
