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
tests = [
  'andor.py',
  'list.py',
  'for-array.py',
  'for-range.py',
  'for-string.py',
  'for-break.py',
  'global.py',
  'if.py',
  'op.py',
  'while.py',
  'while-break.py',
  'while-else.py',
  ]

foreach t: tests
  test('snek   ' + t, snek, workdir: meson.current_source_dir(), args: [t])
  test('python ' + t, prog_python, workdir: meson.current_source_dir(), args: [t])
endforeach

