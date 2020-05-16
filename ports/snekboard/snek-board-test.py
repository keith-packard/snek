#!/usr/bin/env python3
#
# Copyright © 2019–2020 Keith Packard <keithp@keithp.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#

import argparse
import serial
import sys
import threading
import time

port = None


def sync():
    """Send a command to print a percent-sign, wait until it appears"""
    global port

    # wait for the device to get started
    time.sleep(1)
    port.write(b"\x0eprint(chr(37))\n")
    while True:
        data = port.read(1)
        if data == b"%":
            break
    while True:
        data = port.read(1)
        if data == b"\n":
            break


def run_test(send, expect):
    global port

    if not expect:
        print("test %r: " % (send,), end="")
        input()
    else:
        print("test %r" % (send,))

    port.write(send)

    if expect:
        line = port.read_until()
        if line != expect:
            print("test %r failed (got %r expected %r)" % (send, line, expect))
            return False
    else:
        line = port.read_until()

    return True


tests = (
    (b"1+1\n", b"> 2\r\n"),
    (b"talkto(NEOPIXEL); neopixel(((0.1,0.2,0.3),(0.3,0.2,0.1))); 1\n", b"> 1\r\n"),
    (b"talkto(NEOPIXEL); neopixel(((0,0,0),(0,0,0))); 1\n", None),
    (
        b"talkto(M1); on(); talkto(M2); on(); talkto(M3); on(); talkto(M4); on(); 1\n",
        b"> 1\r\n",
    ),
    (
        b"talkto(M1); off(); talkto(M2); off(); talkto(M3); off(); talkto(M4); off(); 1\n",
        None,
    ),
)


def main():
    global port, reader_thread
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--port", help="Serial device")
    args = arg_parser.parse_args()
    port = args.port
    if not port:
        port = "/dev/ttyACM0"
    port = serial.Serial(
        port=port,
        baudrate=115200,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        xonxoff=True,
        rtscts=False,
        dsrdtr=False,
    )
    sync()
    failures = 0
    for test in tests:
        if not run_test(test[0], test[1]):
            failures = failures + 1
    sys.exit(failures)


main()
