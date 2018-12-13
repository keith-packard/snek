project('newt', 'c',
	license : 'GPLv2+',
	default_options : ['buildtype=debug', 'c_std=c99']
       )

add_project_arguments(['-D_DEFAULT_SOURCE'], language: 'c')

flex = find_program('flex')
bison = find_program('bison')

cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : false)

lgen = generator(flex,
		 output : '@PLAINNAME@.c',
		 arguments : ['-o', '@OUTPUT@', '@INPUT@'])

pgen = generator(bison,
		 output : ['@BASENAME@.c', '@BASENAME@.h'],
		 arguments : ['-v', '@INPUT@', '--defines=@OUTPUT1@', '--output=@OUTPUT0@'])

lfiles = lgen.process('newt-lex.l')
pfiles = pgen.process('newt-gram.y')

conf_data = configuration_data()
configure_file(output : 'config.h',
	       configuration : conf_data)

prog_python = import('python').find_installation('python3')

newt_builtin_h = custom_target(
  'newt-builtin.h',
  output : 'newt-builtin.h',
  input : 'newt-builtin.py',
  command : [prog_python, '@INPUT@', '@OUTPUT@']
	    )

sources = [
  'newt-builtin.c',
  'newt-code.c',
  'newt-error.c',
  'newt-for.c',
  'newt-frame.c',
  'newt-func.c',
  'newt-list.c',
  'newt-main.c',
  'newt-memory.c',
  'newt-name.c',
  'newt-poly.c',
  'newt-string.c',
  newt_builtin_h
]

newt = executable('newt',
		  sources, lfiles, pfiles,
		  dependencies: [m_dep]
		 )

subdir('test')
