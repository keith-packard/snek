project('newt', 'c',
	license : 'GPLv2+',
	default_options : ['buildtype=debug', 'c_std=c99']
       )

add_project_arguments(['-D_DEFAULT_SOURCE'], language: 'c')

lola = find_program('lola')

cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : false)

lgen = generator(lola,
		 output : '@BASENAME@.h',
		 arguments : ['-o', '@OUTPUT@', '@INPUT@'])

lfiles = lgen.process('newt-gram.ll')

conf_data = configuration_data()
configure_file(output : 'config.h',
	       configuration : conf_data)

prog_python = import('python').find_installation('python3')

builtins = [
  'newt-keyword.builtin',
  'newt-base.builtin',
  'newt-posix.builtin'
  ]

newt_builtin_h = custom_target(
  'newt-builtin.h',
  output : 'newt-builtin.h',
  input : ['newt-builtin.py'] + builtins,
  command : [prog_python, '@INPUT@', '-o', '@OUTPUT@']
	    )

sources = [
  'newt-builtin.c',
  'newt-code.c',
  'newt-error.c',
  'newt-for.c',
  'newt-frame.c',
  'newt-func.c',
  'newt-lex.c',
  'newt-list.c',
  'newt-main.c',
  'newt-memory.c',
  'newt-name.c',
  'newt-parse.c',
  'newt-poly.c',
  'newt-posix.c',
  'newt-string.c',
  newt_builtin_h
]

newt = executable('newt',
		  sources, lfiles,
		  dependencies: [m_dep]
		 )

subdir('test')
