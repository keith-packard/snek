#!/usr/bin/python3
#
# Copyright © 2019 Keith Packard <keithp@keithp.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#

import sys
import argparse
import time
import curses
import select
import threading
import serial

from curses.textpad import Textbox, rectangle

global_lock = threading.Lock()
global_lock.acquire()

current_window = False

def my_getch(window):
    global global_lock
    if current_window:
        current_window.set_cursor()
    global_lock.release()
    c = window.getch()
    global_lock.acquire()
    return c

class SnekDevice:
    """Link to snek device"""

    serial = False
    reader_thread = False
    writer_thread = False
    alive = False
    _reader_alive = False

    interface = False

    write_queue = False

    #
    # The interface needs to have a condition variable (cv) that is
    # signaled when data are available to write and function (receive)
    # that gets data that are read
    #

    def __init__(self, device, interface):
        self.interface = interface
        self.serial = serial.Serial(port=device,
                                    baudrate=38400,
                                    bytesize=serial.EIGHTBITS,
                                    parity=serial.PARITY_NONE,
                                    stopbits=serial.STOPBITS_ONE,
                                    xonxoff=True,
                                    rtscts=False,
                                    dsrdtr=False)
        
    def _stop_reader(self):
        """Stop reader thread only, wait for clean exit of thread"""
        self._reader_alive = False
        if hasattr(self.serial, 'cancel_read'):
            self.serial.cancel_read()
        self.receiver_thread.join()

    def start(self):
        """start worker threads"""

        self.alive = True
        self._reader_alive = True

        # start threads

        self.receiver_thread = threading.Thread(target=self.reader, name='rx')
        self.receiver_thread.daemon = True
        self.receiver_thread.start()

        self.transmitter_thread = threading.Thread(target=self.writer, name='tx')
        self.transmitter_thread.daemon = True
        self.transmitter_thread.start()

    def stop(self):
        """set flag to stop worker threads"""
        self.alive = False

    def join(self, transmit_only=False):
        """wait for worker threads to terminate"""
        self.transmitter_thread.join()
        if not transmit_only:
            if hasattr(self.serial, 'cancel_read'):
                self.serial.cancel_read()
            self.receiver_thread.join()

    def close(self):
        self.serial.write(b'\x0f')
        self.serial.close()

    def reader(self):
        """loop and copy serial->"""
        try:
            while self.alive and self._reader_alive:
                # read all that is there or wait for one byte
                data = self.serial.read(self.serial.in_waiting or 1)
                if data:
                    self.interface.receive(str(data, encoding='utf-8', errors='ignore'))
        except serial.SerialException:
            self.alive = False
            raise       # XXX handle instead of re-raise?

    def writer(self):
        """Copy queued data to the serial port."""
        try:
            while self.alive:
                send_data = ""
                with self.interface.cv:
                    while not self.write_queue:
                        self.interface.cv.wait()
                    send_data = self.write_queue
                    self.write_queue = False
                self.serial.write(send_data.encode('utf-8'))
        except:
            self.alive = False
            raise

    def write(self, data):
        if self.write_queue:
            self.write_queue += data
        else:
            self.write_queue = data
        self.interface.cv.notify()


class EditWin:
    """Editable text object"""

    window = 0
    lines = 0
    point = 0
    top_line = 0
    tab_width = 4
    text = ""

    def __init__(self, lines, cols, y, x):
        self.lines = lines
        self.window = curses.newwin(lines, cols, y, x)
        self.window.keypad(True)

    def set_text(self, text):
        self.text = text
        self.point = 0
        self.top_line = 0

    # Convert text index to x/y coord
    
    def point_to_cursor(self, point):
        line = -1
        col = 0
        for s in self.text[:point].split('\n'):
            line += 1
            col = len(s)
        return(col, line)

    # Convert x/y coord to text index
    
    def cursor_to_point(self, cursor):
        (cur_col, cur_line) = cursor
        if cur_line < 0:
            cur_line = 0
        elif cur_line >= len(self.text.split('\n')):
            cur_line = len(self.text.split('\n')) - 1
        bol = 0
        line = 0
        point = 0
        for s in self.text.split('\n'):
            point = bol + min(cursor[0], len(s))
            if line == cur_line:
                break
            line += 1
            bol += len(s) + 1
        return point

    # Make sure current point is visible in the window

    def scroll_to_point(self):
        while self.point_to_cursor(self.point)[1] < self.top_line:
            self.top_line -= 1
        while self.point_to_cursor(self.point)[1] >= self.top_line + self.lines:
            self.top_line += 1

    # Repaint the window

    def repaint(self):
        self.window.erase()
        self.scroll_to_point()
        line = 0
        for s in self.text.split('\n'):
            if self.top_line <= line and line < self.top_line + self.lines:
                self.window.addstr(line - self.top_line, 0, s)
            line += 1
        self.window.refresh()

    def set_cursor(self):
        p = self.point_to_cursor(self.point)
        self.window.move(p[1] - self.top_line, p[0])
        self.window.refresh()

    # Find the indent of the specified line

    def indent_at(self, line):
        bol = self.cursor_to_point((0, line))
        point = bol
        while point < len(self.text) and self.text[point] == " ":
            point += 1
        return point - bol

    # Return the last character on the specified line

    def last_ch(self, line):
        eol = self.cursor_to_point((65536, line))
        point = eol - 1
        while point > 0 and self.text[point] == " ":
            point -= 1
        return self.text[point]

    # Is 'point' in the indentation of its line?

    def in_indent(self, point):
        cursor = self.point_to_cursor(point)
        if cursor[0] > 0 and self.indent_at(cursor[1]) >= cursor[0]:
            return True
        return False

    # Move left

    def left(self):
        if self.point > 0:
            self.point -= 1

    # Move up

    def up(self):
        pos = self.point_to_cursor(self.point)
        point = self.cursor_to_point((pos[0], pos[1]-1))
        if point == self.point:
            point = self.cursor_to_point((0, pos[1]))
        self.point = point

    # Move right

    def right(self):
        if self.point < len(self.text):
            self.point += 1

    # Move down

    def down(self):
        pos = self.point_to_cursor(self.point)
        point = self.cursor_to_point((pos[0], pos[1]+1))
        if point == self.point:
            point = self.cursor_to_point((65536, pos[1]))
        self.point = point

    # Move to begining of line

    def bol(self):
        pos = self.point_to_cursor(self.point)
        self.point = self.cursor_to_point((0, pos[1]))

    def eol(self):
        pos = self.point_to_cursor(self.point)
        self.point = self.cursor_to_point((65536, pos[1]))

    def insert(self, text):
        self.text = self.text[:self.point] + text + self.text[self.point:]
        self.point += len(text)

    def delete(self, count):
        self.text = self.text[:self.point] + self.text[self.point + count:]

    def backtab(self):
        pos = self.point_to_cursor(self.point)
        if pos[0] == 0:
            return
        to_remove = pos[0] % self.tab_width
        if to_remove == 0:
            to_remove = self.tab_width
        self.point -= to_remove
        self.delete(to_remove)

    def backspace(self):
        if self.point > 0:
            if self.in_indent(self.point):
                self.backtab()
            else:
                self.left()
                self.delete(1)

    def indent(self, want):
        self.bol()
        have = 0
        while self.point < len(self.text) and self.text[self.point] == " ":
            self.right()
            have += 1
        if have < want:
            self.insert(" " * (want - have))
        elif have > want:
            self.delete(have - want)

    def auto_indent(self):
        cursor = self.point_to_cursor(self.point)
        want = 0
        if cursor[1] > 0:
            want = self.indent_at(cursor[1] - 1)
            if self.last_ch(cursor[1] - 1) == ":":
                want += self.tab_width
        self.indent(want)

    def getch(self):
        self.repaint()
        return my_getch(self.window)

    def prev_line(self):
        pos = self.point_to_cursor(self.point)
        if pos[1] == 0:
            return ""
        start = self.cursor_to_point((0, pos[1]-1))
        end = self.cursor_to_point((0, pos[1]))
        return self.text[start:end]

    def dispatch(self, ch):
        if ch == curses.KEY_LEFT:
            self.left()
        elif ch == curses.KEY_RIGHT:
            self.right()
        elif ch == curses.KEY_UP:
            self.up()
        elif ch == curses.KEY_DOWN:
            self.down()
        elif ch == curses.KEY_HOME:
            self.bol()
        elif ch == curses.KEY_END:
            self.eol()
        elif ch == ord('\t'):
            self.auto_indent()
        elif ch in (curses.ascii.BS, curses.KEY_BACKSPACE, curses.ascii.DEL):
            self.backspace()
        elif curses.ascii.isprint(ch) or ch == ord('\n'):
            self.insert(chr(ch))

stdscr = 0
edit_win = 0
repl_win = 0

snek_device = False

edit_lines = 0
repl_lines = 0
repl_y = 0

def set_sizes():
    global repl_lines, edit_lines, repl_y
    repl_lines = curses.LINES // 3
    edit_lines = curses.LINES - repl_lines - 1
    repl_y = edit_lines + 1

def screen_init():
    global stdscr, edit_win, repl_win
    stdscr = curses.initscr()
    curses.noecho()
    curses.cbreak()
    stdscr.keypad(True)
    set_sizes()
    edit_win = EditWin(edit_lines, curses.COLS, 0, 0)
    repl_win = EditWin(repl_lines, curses.COLS, repl_y, 0)
    stdscr.addstr(edit_lines, 0, "-" * curses.COLS)
    stdscr.refresh()

def screen_fini():
    global stdscr
    stdscr.keypad(False)
    curses.nocbreak()
    curses.echo()
    curses.endwin()

def snekde_init():
    screen_init()

def snekde_fini():
    screen_fini()

def snekde_get_text():
    edit_win.set_text("")
    snek_device.write("\x0eeeprom.show(1)\n")

def snekde_put_text():
    snek_device.write("\x0eeeprom.write()\n")
    snek_device.write(edit_win.text)
    snek_device.write('\x04')

def run():
    global current_window
    current_window = edit_win
    while True:
        ch = current_window.getch()
        if ch == curses.KEY_NPAGE or ch == curses.KEY_PPAGE:
            if current_window is edit_win:
                current_window = repl_win
            else:
                current_window = edit_win
            continue
        if ch == curses.KEY_F1:
            if snek_device:
                snek_device.write(chr(3))
        elif ch == curses.KEY_F2:
            if snek_device:
                snekde_get_text()
        elif ch == curses.KEY_F3:
            if snek_device:
                snekde_put_text()
        elif ch == curses.KEY_F4:
            sys.exit(0)
        else:
            current_window.dispatch(ch)
            if ch == ord('\n'):
                if current_window is edit_win:
                    current_window.auto_indent()
                elif snek_device:
                    data = repl_win.prev_line()
                    if data[:2] == "> ":
                        data = data[2:]
                    snek_device.write('\x0e' + data)

class SnekMonitor:

    def __init__(self):
        self.cv = threading.Condition(global_lock)

    getting_text = False

    def add_to(self, window, data):
        follow = window == repl_win and window.point == len(window.text)
        window.text += data
        if follow:
            window.point += len(data)
        window.repaint()
        if current_window:
            current_window.set_cursor()

    def receive(self, data):
        global repl_win
        data_edit = ""
        data_repl = ""
        for c in data:
            if c == '\x02':
                self.getting_text = True
            elif c == '\x03':
                self.getting_text = False
            elif c == '\x00':
                continue
            elif c == '\r':
                continue
            else:
                if self.getting_text:
                    data_edit += c
                else:
                    data_repl += c
        global_lock.acquire()
        if data_edit:
            self.add_to(edit_win, data_edit)
        if data_repl:
            self.add_to(repl_win, data_repl)
        global_lock.release()

def main():
    global snek_device

    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--list", action='store_true', help="List available serial devices")
    arg_parser.add_argument("--port", help="Serial device")
    args = arg_parser.parse_args()
    snek_device = False
    if args.port:
        snek_monitor = SnekMonitor()
        snek_device = SnekDevice(args.port, snek_monitor)
    try:
        snekde_init()
        if snek_device:
            snek_device.start()
        run()
    finally:
        snekde_fini()
    print("source %s" % edit_win.text)

main()
