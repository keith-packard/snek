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

