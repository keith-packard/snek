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
project('snek', 'c',
	license : 'GPLv2+',
	default_options : ['buildtype=debug', 'c_std=c99']
       )

add_project_arguments(['-D_DEFAULT_SOURCE', '-DSNEK_MEM_INCLUDE_NAME'], language: 'c')

lola = find_program('lola')

cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : false)

lgen = generator(lola,
		 output : '@BASENAME@.h',
		 arguments : ['-o', '@OUTPUT@', '@INPUT@'])

lfiles = lgen.process('snek-gram.ll')

conf_data = configuration_data()
configure_file(output : 'config.h',
	       configuration : conf_data)

prog_python = import('python').find_installation('python3')

builtins = [
  'snek-keyword.builtin',
  'snek-base.builtin',
  'snek-posix.builtin'
  ]

snek_builtin_h = custom_target(
  'snek-builtin.h',
  output : 'snek-builtin.h',
  input : ['snek-builtin.py'] + builtins,
  command : [prog_python, '@INPUT@', '-o', '@OUTPUT@']
	    )

sources = [
  'snek-builtin.c',
  'snek-code.c',
  'snek-error.c',
  'snek-frame.c',
  'snek-func.c',
  'snek-lex.c',
  'snek-list.c',
  'snek-main.c',
  'snek-memory.c',
  'snek-name.c',
  'snek-parse.c',
  'snek-poly.c',
  'snek-posix.c',
  'snek-print.c',
  'snek-string.c',
  snek_builtin_h
]

snek = executable('snek',
		  sources, lfiles,
		  dependencies: [m_dep],
		  install: true
		 )

subdir('test')
