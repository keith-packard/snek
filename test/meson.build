tests = [
  'global.py',
  'if.py',
  'while.py',
  'while-break.py',
  'while-else.py',
  ]

foreach t: tests
  test('newt   ' + t, newt, workdir: meson.current_source_dir(), args: [t])
  test('python ' + t, prog_python, workdir: meson.current_source_dir(), args: [t])
endforeach

