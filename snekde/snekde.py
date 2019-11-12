#!/usr/bin/python3
#
# Copyright © 2019 Keith Packard <keithp@keithp.com>
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

import sys
import argparse
import time
import curses
import threading
import serial
import serial.tools.list_ports

from pathlib import Path, PurePath

from curses import ascii

stdscr = 0

snek_lock = threading.Lock()
snek_lock.acquire()

snek_current_window = 0
snek_edit_win = 0
snek_repl_win = 0

snek_cols = 0
snek_lines = 0

snek_monitor = False

snek_device = False

snek_dialog_waiting = False

snek_cur_path = None

#snek_debug_file = open('log', 'w')

#def snek_debug(message):
#    snek_debug_file.write(message + '\n')
#    snek_debug_file.flush()

#
# Read a character from the keyboard, releasing the
# global lock while blocked so that serial input can arrive
#

def my_getch(edit_win):
    global snek_lock, snek_current_window, snek_dialog_waiting
    while True:
        edit_win.set_cursor()
        snek_lock.release()
        c = edit_win.window.getch()
        if c == ord('\r'):
            c = ord('\n')
        snek_lock.acquire()
        if not snek_dialog_waiting:
            break
        if c == ord('\n'):
            snek_dialog_waiting.close()
            snek_dialog_waiting = False

    #
    # Check for resize
    #
    if c == curses.KEY_RESIZE:
        screen_resize()
    return c

class SnekPort:
    """Emulate new pySerial from old pySerial"""

    name = None
    description = None
    device = None

    def __init__(self, port):
        if type(port) is tuple:
            self.device = port[0]
            if len(port[1]) > 20:
                self.name = self.device
                if '/' in self.name:
                    self.name = self.name[self.name.rfind('/')+1:]
            else:
                self.name = port[1]
            self.description = port[2]
        else:
            self.device = port.device
            if port.name:
                self.name = port.name
            else:
                self.name = self.device
            self.description = port.description
        if self.name and '/' in self.name:
            self.name = self.name[self.name.rfind('/')+1:]

# A special hack for pre-3.5 pySerial
#
# That code is missing a call to 'str' to convert the check_output
# result to a string.

if 'list_ports_posix' in serial.tools.__dict__:
    if hasattr(serial.tools.list_ports_posix, 'popen'):
        del serial.tools.list_ports_posix.popen

        try:
            import subprocess
        except ImportError:
            def popen(argv):
                try:
                    si, so =  os.popen4(' '.join(argv))
                    return so.read().strip()
                except:
                    raise IOError('lsusb failed')
        else:
            def popen(argv):
                try:
                    return str(subprocess.check_output(argv, stderr=subprocess.STDOUT)).strip()
                except:
                    raise IOError('lsusb failed')

        serial.tools.list_ports_posix.popen = popen

def snek_list_ports():
    ports = []
    for port in serial.tools.list_ports.comports():
        ports += [SnekPort(port)]
    return ports

class SnekDevice:
    """Link to snek device"""

    serial = False
    reader_thread = False
    writer_thread = False
    alive = False
    _reader_alive = False
    interface = False
    write_queue = False
    device = ""
    interrupt_pending = False

    #
    # The interface needs to have a condition variable (cv) that is
    # signaled when data are available to write and function (receive)
    # that gets data that are read
    #

    def __init__(self, device, interface):
        self.interface = interface
        self.device = device
        self.serial = serial.Serial(port=device,
                                    baudrate=115200,
                                    bytesize=serial.EIGHTBITS,
                                    parity=serial.PARITY_NONE,
                                    stopbits=serial.STOPBITS_ONE,
                                    xonxoff=True,
                                    rtscts=False,
                                    dsrdtr=False)
        
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

    def stop_reader(self):
        if self.receiver_thread and threading.current_thread() != self.receiver_thread:
            self.serial.cancel_read()
            self.interface.cv.release()
            self.receiver_thread.join()
            self.interface.cv.acquire()

    def stop_writer(self):
        """set flag to stop worker threads"""
        if self.transmitter_thread and threading.current_thread() != self.transmitter_thread:
            self.interface.cv.notify()
            self.interface.cv.release()
            self.transmitter_thread.join()
            self.interface.cv.acquire()

    def close(self):
        self.alive = False
        self.stop_reader()
        self.stop_writer()
        try:
            self.serial.write_timeout = 1
            self.serial.write(b'\x0f')
            self.serial.reset_output_buffer()
            self.serial.xonxoff = False
            self.serial.close()
        except serial.SerialException:
            pass

    def reader(self):
        """loop and copy serial->"""
        try:
            while self.alive and self._reader_alive:
                # read all that is there or wait for one byte
                data = self.serial.read(1)
                if data:
                    self.interface.receive(str(data, encoding='utf-8', errors='ignore'))
        except serial.SerialException as e:
            self.interface.failed(self.device)
        finally:
            self.receiver_thread = False

    def writer(self):
        """Copy queued data to the serial port."""
        try:
            while self.alive:
                send_data = ""
                interrupt = False
                with self.interface.cv:
                    while not self.write_queue and not self.interrupt_pending and self.alive:
                        self.interface.cv.wait()
                    if not self.alive:
                        return
                    send_data = self.write_queue
                    interrupt = self.interrupt_pending
                    self.write_queue = False
                    self.interrupt_pending = False
                if interrupt:
                    self.serial.reset_output_buffer()
                    self.serial.xonxoff = False
                    self.serial.write(b'\x0f\x03\x0e')
                    self.serial.xonxoff = True
                if send_data:
                    self.serial.write(send_data.encode('utf-8'))
        except (serial.SerialException, termios.error):
            self.interface.failed(self.device)
        finally:
            self.transmitter_thread = False

    def interrupt(self):
        self.interrupt_pending = True
        self.interface.cv.notify()

    def write(self, data):
        if self.write_queue:
            self.write_queue += data
        else:
            self.write_queue = data
        self.interface.cv.notify()

    def command(self, data, intr='\x03'):
        self.write("\x0e" + intr + data)

class EditWin:
    """Editable text object"""

    window = 0
    lines = 0
    cols = 0
    y = 0
    point = 0
    top_line = 0
    tab_width = 4
    text = ""
    cut = ""
    mark = -1
    changed = False

    undo = []

    def __init__(self, lines, cols, y, x):
        self.lines = lines
        self.cols = cols
        self.y = y
        self.window = curses.newwin(lines, cols, y, x)
        self.window.keypad(True)
        self.window.notimeout(False)

    # Set contents, resetting state back to start
    
    def set_text(self, text):
        self.text = text
        self.point = 0
        self.mark = -1
        self.top_line = 0
        self.changed = False

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

    def addstr(self, line, col, text, attrib=None):
        if col < self.cols:
            if line == self.lines - 1:
                text = text[:self.cols - col - 1]
            else:
                text = text[:self.cols - col]
            if attrib:
                self.window.addstr(line, col, text, attrib)
            else:
                self.window.addstr(line, col, text)

    # Repaint the window

    def repaint(self):
        self.window.erase()
        self.scroll_to_point()
        selection = self.get_selection()
        if selection:
            start_selection = self.point_to_cursor(selection[0])
            end_selection = self.point_to_cursor(selection[1])
        line = 0
        for s in self.text.split('\n'):
            if self.top_line <= line and line < self.top_line + self.lines:

                # Paint the marked region in reverse video

                if selection and start_selection[1] <= line and line <= end_selection[1]:
                    if line == start_selection[1]:
                        before = s[:start_selection[0]]
                        middle = s[start_selection[0]:]
                    else:
                        before = ""
                        middle = s
                    if line == end_selection[1]:
                        after = middle[end_selection[0]:]
                        middle = middle[:end_selection[0]]
                    else:
                        after = ""
                    self.addstr(line - self.top_line, 0, before)
                    self.addstr(line - self.top_line, len(before), middle, curses.A_REVERSE)
                    self.addstr(line - self.top_line, len(before) + len(middle), after)
                else:
                    self.addstr(line - self.top_line, 0, s)
            line += 1
        self.window.refresh()

    # Set window size

    def resize(self, lines, cols, y, x):
        self.lines = lines
        self.cols = cols
        self.window.resize(lines, cols)
        self.window.mvwin(y, x)
        self.repaint()

    # This window is the input window, set the cursor position
    # to the current point

    def set_cursor(self):
        p = self.point_to_cursor(self.point)
        col = min(self.cols - 1, p[0])
        self.window.move(p[1] - self.top_line, col)
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

    # Move to beginning of line

    def bol(self):
        pos = self.point_to_cursor(self.point)
        self.point = self.cursor_to_point((0, pos[1]))

    # Move to end of line

    def eol(self):
        pos = self.point_to_cursor(self.point)
        self.point = self.cursor_to_point((65536, pos[1]))

    # Move to beginning of buffer

    def bob(self):
        self.point = 0

    # Move to end of buffer

    def eob(self):
        self.point = len(self.text)

    # Move to previous page. That means moving up by self.lines-1 and
    # then placing that line at the bottom of the screen

    def prev_page(self):
        pos = self.point_to_cursor(self.point)
        self.point = self.cursor_to_point((pos[0], pos[1] - (self.lines)-1))
        new_line = self.point_to_cursor(self.point)[1]
        top_line = new_line - (self.lines-1)
        top_pos = self.cursor_to_point((0, top_line))
        self.top_line = self.point_to_cursor(top_pos)[1]

    # Move to next page. This means moving down by self.lines-1 and
    # then placing that line at the top of the screen

    def next_page(self):
        pos = self.point_to_cursor(self.point)
        self.point = self.cursor_to_point((pos[0], pos[1] + (self.lines)-1))
        self.top_line = self.point_to_cursor(self.point)[1]

    # Check whether in the last line of the buffer

    def in_last_line(self):
        end = self.point_to_cursor(len(self.text))
        return self.point >= self.cursor_to_point((0, end[1]))

    def push_undo(self, point, operation):
        self.undo.append((point, operation, self.point, self.mark))

    def pop_undo(self):
        if not self.undo:
            return False
        (point, operation, self_point, self_mark) = self.undo.pop()

        if isinstance(operation, str):
            # Replace deleted text
            self.text = self.text[:point] + operation + self.text[point:]
        else:
            # Delete inserted text
            self.text = self.text[:point] + self.text[point+operation:]

        self.point = self_point
        self.mark = self_mark
        self.changed = True
        return True

    # Insert some text, adjusting self.point and self.mark if the text
    # is before them

    def insert(self, point, text):
        self.push_undo(point, len(text))
        self.text = self.text[:point] + text + self.text[point:]
        if point < self.point:
            self.point += len(text)
        if point < self.mark:
            self.mark += len(text)
        self.changed = True

    def insert_at_point(self, text):
        self.insert(self.point, text)
        self.point += len(text)

    # Delete some text, adjusting self.point if the delete starts
    # before it

    def _adjust_delete_position(self, delete_point, delete_count, moving_point, is_mark):
        if delete_point <= moving_point and moving_point < delete_point + delete_count:
            if is_mark:
                return -1
            moving_point = delete_point
        elif delete_point + delete_count <= moving_point:
            moving_point -= delete_count
        return moving_point

    def delete(self, point, count):
        self.push_undo(point, self.text[point:point+count])
        self.text = self.text[:point] + self.text[point + count:]
        self.point = self._adjust_delete_position(point, count, self.point, False)
        if self.mark >= 0:
            self.mark = self._adjust_delete_position(point, count, self.mark, True)
        self.changed = True

    def delete_at_point(self, count):
        self.delete(self.point, count)

    # Delete back to the previous tab stop

    def backtab(self):
        pos = self.point_to_cursor(self.point)
        if pos[0] == 0:
            return
        to_remove = pos[0] % self.tab_width
        if to_remove == 0:
            to_remove = self.tab_width
        self.point -= to_remove
        self.delete_at_point(to_remove)

    # Delete something. If there's a mark, delete that.  otherwise,
    # delete backwards, if in indent of the line, backtab
    
    def backspace(self):
        selection = self.get_selection()
        if selection:
            self.delete(selection[0], selection[1] - selection[0])
            self.mark = -1
        elif self.point > 0:
            if self.in_indent(self.point):
                self.backtab()
            else:
                self.left()
                self.delete_at_point(1)

    # Set or clear the 'mark', which defines
    # the other end of the current selection

    def toggle_mark(self):
        if self.mark >= 0:
            self.mark = -1
        else:
            self.mark = self.point

    # Return the extent of the current selection, False if none

    def get_selection(self):
        if self.mark >= 0:
            return (min(self.mark, self.point), max(self.mark, self.point))
        else:
            return False

    # Copy from mark to point and place in cut buffer
    # delete from text if requested

    def copy(self, delete=False):
        selection = self.get_selection()
        if selection:
            (start, end) = selection
                
            self.cut = self.text[start:end]
            if delete:
                self.delete(start, end - start)
            self.mark = -1

    # Paste any cut buffer at point
    
    def paste(self):
        if self.cut:
            self.insert_at_point(self.cut)

    # Set indent of current line to 'want'. Leave point
    # at the end of the indent

    def indent(self, want):
        self.bol()
        have = 0
        while self.point < len(self.text) and self.text[self.point] == " ":
            self.right()
            have += 1
        if have < want:
            self.insert_at_point(" " * (want - have))
        elif have > want:
            self.delete_at_point(have - want)

    # Automatically indent the current line,
    # using the previous line as a guide

    def auto_indent(self):
        cursor = self.point_to_cursor(self.point)
        want = 0
        if cursor[1] > 0:
            want = self.indent_at(cursor[1] - 1)
            if self.last_ch(cursor[1] - 1) == ":":
                want += self.tab_width
        self.indent(want)

    # Delete to end of line, or delete newline if at end of line

    def delete_to_eol(self):
        current = self.point_to_cursor(self.point)
        eol = self.cursor_to_point((65536, current[1]))
        if self.point == eol:
            self.delete_at_point(1)
        else:
            self.delete_at_point(eol - self.point)

    # Read a character for this window

    def getch(self):
        self.repaint()
        return my_getch(self)

    # Return the contents of the previous line

    def cur_line(self):
        pos = self.point_to_cursor(self.point)
        start = self.cursor_to_point((0, pos[1]))
        end = self.cursor_to_point((65536, pos[1]))
        return self.text[start:end]

    def dispatch(self, ch):
        if ch == 0 or ch == ord(' ') + 0x80:
            self.toggle_mark()
        elif ch == ord('c') & 0x1f or ch == ord('c') + 0x80:
            self.copy(delete=False)
        elif ch == ord('x') & 0x1f or ch == ord('x') + 0x80:
            self.copy(delete=True)
        elif ch == ord('v') & 0x1f or ch == ord('v') + 0x80:
            self.paste()
        elif ch == ord('k') & 0x1f:
            self.delete_to_eol()
        elif ch == ord('z') & 0x1f:
            self.pop_undo()
        elif ch == curses.KEY_LEFT or ch == ord('b') & 0x1f:
            self.left()
        elif ch == curses.KEY_RIGHT or ch == ord('f') & 0x1f:
            self.right()
        elif ch == curses.KEY_UP or ch == ord('p') & 0x1f:
            self.up()
        elif ch == curses.KEY_DOWN or ch == ord('n') & 0x1f:
            self.down()
        elif ch == curses.KEY_HOME or ch == ord('a') & 0x1f:
            self.bol()
        elif ch == curses.KEY_PPAGE:
            self.prev_page()
        elif ch == curses.KEY_NPAGE:
            self.next_page()
        elif ch == curses.KEY_END or ch == ord('e') & 0x1f:
            self.eol()
        elif ch == ord('\t'):
            self.auto_indent()
        elif ch in (curses.ascii.BS, curses.KEY_BACKSPACE, curses.ascii.DEL):
            self.backspace()
        elif curses.ascii.isprint(ch) or ch == ord('\n'):
            self.insert_at_point(chr(ch))

class ErrorWin:
    """Show an error message"""
    label = ""
    x = 0
    y = 0
    nlines = 5
    ncols = 40
    inputthread = True

    window = False

    def __init__(self, label, inputthread=True):
        self.label = label
        self.inputthread = inputthread
        self.ncols = min(snek_cols, max(40, len(label) + 2))
        self.x = (snek_cols - self.ncols) // 2
        self.y = (snek_lines - self.nlines) // 2
        self.window = curses.newwin(self.nlines, self.ncols, self.y, self.x)
        self.window.keypad(True)
        self.window.notimeout(False)
        self.run_dialog()

    def repaint(self):
        self.window.border()
        l = len(self.label)
        if l > self.ncols:
            l = self.ncols
        self.window.addstr(1, (self.ncols - l) // 2, self.label)
        self.window.addstr(3, 2, "OK")

    def close(self):
        del self.window
        screen_repaint()

    def run_dialog(self):
        global snek_dialog_waiting
        self.repaint()
        self.window.move(3, 4)
        self.window.refresh()
        if self.inputthread:
            self.window.getstr()
            self.close()
        else:
            snek_dialog_waiting = self

class GetTextWin:
    """Prompt for line of text"""

    label = ""
    prompt = ""
    x = 0
    y = 0
    nlines = 5
    ncols = 40

    window = False

    def __init__(self, label, prompt="File:"):
        self.label = label
        self.prompt = prompt
        self.x = (snek_cols - self.ncols) // 2
        self.y = (snek_lines - self.nlines) // 2
        self.window = curses.newwin(self.nlines, self.ncols, self.y, self.x)
        self.window.keypad(True)
        self.window.notimeout(False)

    def repaint(self):
        self.window.border()
        self.window.addstr(1, (self.ncols - len(self.label)) // 2, self.label)
        self.window.addstr(3, 2, self.prompt)

    def run_dialog(self):
        self.repaint()
        self.window.move(3, 8)
        curses.echo()
        name = self.window.getstr()
        curses.noecho()
        del self.window
        screen_repaint()
        return str(name, encoding='utf-8', errors='ignore')

class GetFileWin:
    """Prompt for a filename"""
    label = ""
    dir = ""
    x = 0
    y = 0
    nlines = 20
    ncols = 60
    new = False
    window = False
    cur_ent = 1
    file = ""
    full_file = ""

    def __init__(self, label, path, new=False):
        self.label = label
        self.new = new
        self.nlines = snek_lines - 2
        self.ncols = snek_cols - 10
        self.cur_ent = 1
        self.top_ent = 0
        self.x = (snek_cols - self.ncols) // 2
        self.y = (snek_lines - self.nlines) // 2
        self.window = curses.newwin(self.nlines, self.ncols, self.y, self.x)
        self.window.keypad(True)
        self.window.notimeout(False)
        if path:
            self.dir = path.parent
        else:
            self.dir = Path().resolve()
        self.get_files()
        if path:
            for i in range(len(self.files)):
                if self.files[i] == path.name:
                    self.set_ent(i)
                    break

    def get_files(self):
        files = []
        dirs = []
        for f in self.dir.glob('*'):
            if f.is_dir():
                dirs += [f.name]
            else:
                files += [f.name]
        dirs.sort()
        files.sort()
        self.files = ['..'] + dirs + files
        self.file_offset = 0
        self.cur_ent = 1
        self.file = self.files[1]

    def set_dir(self, dir):
        self.dir = self.path.resolve()
        self.get_files()
        
    def ent_line(self, i):
        return 4 + i

    def ents(self):
        return self.nlines - 5

    def set_ent(self, ent):
        if 0 <= ent and ent < len(self.files):
            self.cur_ent = ent
            self.file = self.files[self.cur_ent]
            if ent < self.file_offset:
                self.file_offset = ent
            elif ent >= self.file_offset + self.ents():
                self.file_offset = ent - self.ents() + 1

    def repaint(self):
        self.window.border()
        self.window.addstr(1, (self.ncols - len(self.label)) // 2, self.label)
        self.path = self.dir / PurePath(self.file)
        if self.file == '':
            self.full_file = str(self.dir / PurePath('x'))[:-1]
        else:
            self.full_file = str(self.path)
        self.window.addstr(2, 2, ' ' * (self.ncols - 3))
        left = max(0, len(self.full_file) - 2 - self.ncols)
        self.window.addstr(2, 2, self.full_file[left:])
        for i in range(0,self.ents()):
            f = self.file_offset + i
            self.window.addstr(self.ent_line(i), 2, ' ' * (self.ncols - 3))
            if f < len(self.files):
                text = self.files[f]
                if (self.dir / PurePath(text)).is_dir():
                    text = '[' + text + ']'
                text = text[0:self.ncols-3]
                if f == self.cur_ent:
                    self.window.addstr(self.ent_line(i), 2, text, curses.A_REVERSE)
                else:
                    self.window.addstr(self.ent_line(i), 2, text)

    def set_cursor(self):
        self.window.move(2, min(2 + len(self.full_file), self.ncols - 2))
        self.window.refresh()

    def run_dialog(self):
        while True:
            self.repaint()
            self.window.refresh()
            ch = my_getch(self)
            if ch == ord('\n'):
                if not self.path.is_dir():
                    break;
                self.set_dir(self.path.resolve())
            elif ch == curses.ascii.ESC or ch == curses.KEY_F4:
                self.path = None
                break
            elif ch == curses.KEY_UP or ch == ord('p') & 0x1f:
                self.set_ent(self.cur_ent - 1)
            elif ch == curses.KEY_DOWN or ch == ord('n') & 0x1f:
                self.set_ent(self.cur_ent + 1)
            else:
                if not self.new:
                    curses.beep()
                elif ch in (curses.ascii.BS, curses.KEY_BACKSPACE, curses.ascii.DEL):
                    if len(self.file) > 0:
                        self.file = self.file[0:-1]
                elif curses.ascii.isprint(ch):
                    self.file = self.file + chr(ch)
                else:
                    curses.beep()
                    
        del self.window
        screen_repaint()
        return self.path

class GetPortWin:
    """Prompt for serial port"""

    ports = False
    x = 0
    y = 0
    nlines = 0
    ncols = 40
    curline = 1
    fakeport = None
    title = None

    def label(self, port):
        l = "%-15.15s %s" % (port.name, port.description)
        return l[:snek_cols-3]

    def make_fake(self, description):
        return SnekPort(("Cancel", "Cancel", description))

    def __init__(self, title="Select Port"):
        self.title = title
        self.ports = snek_list_ports()
        if len(self.ports) == 0:
            self.fakeport = self.make_fake("No ports available")
        else:
            self.fakeport = self.make_fake("")
        maxlen = len(self.title)
        for port in self.ports:
            maxlen = max(maxlen, len(self.label(port)))
        maxlen = max(maxlen, len(self.label(self.fakeport)))
        self.ncols = maxlen + 3
        self.nlines = 2 + 1 + len(self.ports) + 1
        self.curline = 0
        self.x = (snek_cols - self.ncols) // 2
        self.y = (snek_lines - self.nlines) // 2
        
        self.window = curses.newwin(self.nlines, self.ncols, self.y, self.x)
        self.window.keypad(True)
        self.window.notimeout(False)

    def paint_port(self, line, port):
        if line == self.curline:
            self.window.addstr(line+2, 1, " " * (self.ncols - 3), curses.A_REVERSE)
            self.window.addstr(line+2, 1, self.label(port), curses.A_REVERSE)
        else:
            self.window.addstr(line+2, 1, " " * (self.ncols - 3))
            self.window.addstr(line+2, 1, self.label(port))

    def repaint(self):
        self.window.border()
        self.window.addstr(1, (self.ncols - len(self.title)) // 2, self.title)
        line = 0
        for port in self.ports:
            self.paint_port(line, port)
            line += 1
        self.paint_port(line, self.fakeport)

    def set_cursor(self):
        self.window.move(self.curline + 2,self.ncols-2)

    def run_dialog(self):
        while True:
            self.repaint()
            ch = my_getch(self)
            if ch == ord('\n'):
                del self.window
                screen_repaint()
                if self.curline >= len(self.ports):
                    return None
                return self.ports[self.curline].device
            elif ch == curses.KEY_UP or ch == ord('p') & 0x1f:
                self.curline = max(0, self.curline-1)
            elif ch == curses.KEY_DOWN or ch == ord('n') & 0x1f:
                self.curline = min(len(self.ports), self.curline+1)

def screen_get_sizes():
    global snek_lines, snek_cols, stdscr
    snek_lines, snek_cols = stdscr.getmaxyx()
    repl_lines = snek_lines // 3
    edit_lines = snek_lines - repl_lines - 2
    edit_y = 1
    repl_y = edit_y + edit_lines + 1
    return (edit_lines, edit_y, repl_lines, repl_y)

help_text = (
    ("F1", "Device"),
    ("F2", "Get"),
    ("F3", "Put"),
    ("F4", "Quit"),
    ("F5", "Load"),
    ("F6", "Save"),
    ("F7", "Switch")
    )

# Paint the function key help text and the separator line

def screen_paint():
    global stdscr, snek_device, snek_edit_win
    help_col = 0
    help_cols = min(snek_cols // len(help_text), 13)
    stdscr.addstr(0, 0, " " * snek_cols)
    for (f, t) in help_text:
        stdscr.addstr(0, help_col, " %2s: %-6s " % (f, t), curses.A_REVERSE)
        help_col += help_cols
    device_name = "<no device>"
    if snek_device:
        device_name = snek_device.device
    device_col = snek_cols - len(device_name)
    if device_col < 0:
        device_col = 0
    mid_y = snek_edit_win.y + snek_edit_win.lines
    stdscr.addstr(mid_y, device_col, device_name, curses.A_REVERSE)
    if device_col >= 6:
        stdscr.addstr(mid_y, device_col - 6, "      ", curses.A_REVERSE)
    for col in range(0,device_col - 6,5):
        stdscr.addstr(mid_y, col, "snek ", curses.A_REVERSE)
    stdscr.refresh()
    
# Repaint everything, as when a dialog goes away

def screen_repaint():
    global snek_edit_win, snek_repl_win
    stdscr.clear()
    snek_edit_win.repaint()
    snek_repl_win.repaint()
    screen_paint()
    if snek_current_window:
        snek_current_window.set_cursor()

# Handle screen resize

def screen_resize():
    global snek_edit_win, snek_repl_win
    #
    # Update desired window sizes
    #
    (edit_lines, edit_y, repl_lines, repl_y) = screen_get_sizes()
    stdscr.clear()
    snek_edit_win.resize(edit_lines, snek_cols, edit_y, 0)
    snek_repl_win.resize(repl_lines, snek_cols, repl_y, 0)
    screen_paint()

def screen_init(text):
    global stdscr, snek_edit_win, snek_repl_win
    stdscr = curses.initscr()
    curses.noecho()
    curses.raw()
    stdscr.keypad(True)
    stdscr.notimeout(False)
    (edit_lines, edit_y, repl_lines, repl_y) = screen_get_sizes()
    snek_edit_win = EditWin(edit_lines, snek_cols, edit_y, 0)
    if text:
        snek_edit_win.set_text(text)
    snek_repl_win = EditWin(repl_lines, snek_cols, repl_y, 0)
    screen_paint()

def screen_fini():
    global stdscr
    stdscr.keypad(False)
    curses.noraw()
    curses.echo()
    curses.endwin()

def snekde_open_device():
    global snek_device, snek_monitor
    dialog = GetPortWin()
    name = dialog.run_dialog()
    if not name:
        return
    try:
        device = SnekDevice(name, snek_monitor)
        device.start()
        if snek_device:
            snek_device.close()
            del snek_device
        snek_device = device
        screen_paint()
    except OSError as e:
        message = e.strerror
        if not message:
            message = "failed"
        ErrorWin("%s: %s" % (name, message))

def snekde_get_text():
    global snek_edit_win, snek_device
    snek_edit_win.set_text("")
    snek_device.command("eeprom.show(1)\n")

def snekde_put_text():
    global snek_edit_win, snek_device
    if len(snek_edit_win.text.strip()) == 0:
        ErrorWin("No program to put")
        return
    snek_device.command("eeprom.write()\n")
    snek_device.write(snek_edit_win.text + '\x04')
    snek_device.command("reset()\n", intr='')
    snek_edit_win.changed = False

#
# Load a path into the edit buffer
#
def load_file(path):
    global snek_cur_path
    with open(path, 'r') as myfile:
        data = myfile.read()
        snek_edit_win.set_text(data)
        snek_cur_path = path
        snek_edit_win.changed = False

#
# Save the edit buffer to a path
#
def save_file(path):
    global snek_cur_path
    with open(path, 'w') as myfile:
        myfile.write(snek_edit_win.text)
        snek_cur_path = path
        snek_edit_win.changed = False

#
# Display a dialog box requesting a file name,
# then load that file
# 
def snekde_load_file():
    global snek_edit_win, snek_cur_path
    dialog = GetFileWin("Load File", snek_cur_path, new=False)
    path = dialog.run_dialog()
    if not path:
        return
    try:
        load_file(path)
    except OSError as e:
        ErrorWin("%s: %s" % (e.filename, e.strerror))
        
#
# Display a dialog box requesting a file name,
# then save to that file
# 
def snekde_save_file():
    global snek_edit_win, snek_cur_path
    dialog = GetFileWin("Save File", snek_cur_path, new=True)
    path = dialog.run_dialog()
    if not path:
        return
    if path != snek_cur_path and path.exists():
        ErrorWin("%s: already exists" % str(path))
    try:
        save_file(path)
    except OSError as e:
        ErrorWin("%s: %s" % (e.filename, e.strerror))

def run():
    global snek_current_window, snek_edit_win, snek_repl_win, snek_device
    snek_current_window = snek_edit_win
    prev_exit = False
    while True:
        ch = snek_current_window.getch()
        if ch == curses.ascii.ESC:
            ch = snek_current_window.getch() | 0x80
        if ch == 3:
            if snek_device:
                snek_device.interrupt()
        elif ch == curses.KEY_F1 or ch == ord('1') | 0x80:
            snekde_open_device()
        elif ch == curses.KEY_F2 or ch == ord('2') | 0x80:
            if snek_device:
                snekde_get_text()
            else:
                ErrorWin("No device")
        elif ch == curses.KEY_F3 or ch == ord('3') | 0x80:
            if snek_device:
                snekde_put_text()
            else:
                ErrorWin("No device")
        elif ch == curses.KEY_F4 or ch == ord('4') | 0x80:
            if snek_edit_win.changed and not prev_exit:
                ErrorWin("Unsaved changes, quit again to abandon them")
                prev_exit = True
                continue
            sys.exit(0)
        elif ch == curses.KEY_F5 or ch == ord('5') | 0x80:
            snekde_load_file()
        elif ch == curses.KEY_F6 or ch == ord('6') | 0x80:
            snekde_save_file()
        elif ch == curses.KEY_F7 or ch == ord('7') | 0x80 or ch == ord('o') & 0x1f:
            if snek_current_window is snek_edit_win:
                snek_current_window = snek_repl_win
            else:
                snek_current_window = snek_edit_win
        elif ch == ord('\n'):
            if snek_current_window is snek_edit_win:
                snek_current_window.dispatch(ch)
                snek_current_window.auto_indent()
            elif snek_device:
                data = snek_repl_win.cur_line()
                #
                # Trim off snek prompts
                #
                while True:
                    if data[:2] == "> " or data[:2] == "+ ":
                        data = data[2:]
                    elif data[:1] == ">" or data[:1] == "+":
                        data = data[1:]
                    else:
                        break
                #
                # If we're not at the end of the buffer, copy
                # the line to the end
                #
                if not snek_repl_win.in_last_line():
                    snek_repl_win.eob()
                    snek_repl_win.insert_at_point(data)
                snek_repl_win.eob()
                snek_current_window.dispatch(ch)
                data += '\n'
                snek_device.command(data,intr='')
        else:
            snek_current_window.dispatch(ch)
        prev_exit = False


# Class to monitor the serial device for data and
# place in approprite buffer. Will be used as
# parameter to SnekDevice, and so it must expose
# 'cv' as a condition variable and 'receive' as a
# function to get data

class SnekMonitor:

    def __init__(self):
        global snek_lock
        self.cv = threading.Condition(snek_lock)

    # Reading text to snek_edit_win instead of snek_repl_win

    getting_text = False

    def add_to(self, window, data):
        global snek_current_window, snek_repl_win
        follow = window == snek_repl_win and window.point == len(window.text)
        window.text += data
        if follow:
            window.point += len(data)
        window.repaint()
        if snek_current_window:
            snek_current_window.set_cursor()

    def receive(self, data):
        global snek_edit_win, snek_repl_win, snek_lock
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
        with snek_lock:
            if data_edit:
                self.add_to(snek_edit_win, data_edit)
            if data_repl:
                self.add_to(snek_repl_win, data_repl)

    def failed(self, device):
        global snek_device, snek_lock
        with snek_lock:
            if snek_device:
                snek_device.close()
                del snek_device
                snek_device = False
            ErrorWin("Device %s failed" % device, inputthread=False)

def main():
    global snek_device, snek_edit_win, snek_monitor

    snek_monitor = SnekMonitor()

    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--list", action='store_true', help="List available serial devices")
    arg_parser.add_argument("--port", help="Serial device")
    arg_parser.add_argument("file", nargs="*", help="Read file into edit window")
    args = arg_parser.parse_args()
    snek_device = False
    if args.list:
        for port in snek_list_ports():
            print("%-30.30s %s" % (port.name, port.description))
        sys.exit(0)
    if args.port:
        try:
            snek_device = SnekDevice(args.port, snek_monitor)
        except OSError as e:
            print(e.strerror, file=sys.stderr)
            sys.exit(1)
    text = ""
    if args.file:
        try:
            with open(args.file[0], 'r') as myfile:
                text = myfile.read()
        except OSError as e:
            print("%s: %s", (e.filename, e.strerror), file=sys.stderr)
            sys.exit(1)
    try:
        screen_init(text)
        if snek_device:
            snek_device.start()
        run()
    finally:
        screen_fini()

main()
