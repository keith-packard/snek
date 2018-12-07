project('newt', 'c',
	license : 'GPLv2+',
	default_options : ['buildtype=debug', 'c_std=c99']
       )

add_project_arguments(['-D_DEFAULT_SOURCE'], language: 'c')

flex = find_program('flex')
bison = find_program('bison')

lgen = generator(flex,
		 output : '@PLAINNAME@.c',
		 arguments : ['-o', '@OUTPUT@', '@INPUT@'])

pgen = generator(bison,
		 output : ['@BASENAME@.c', '@BASENAME@.h'],
		 arguments : ['@INPUT@', '--defines=@OUTPUT1@', '--output=@OUTPUT0@'])

lfiles = lgen.process('newt-lex.l')
pfiles = pgen.process('newt-gram.y')

conf_data = configuration_data()
configure_file(output : 'config.h',
	       configuration : conf_data)

cfiles=[
  'newt-code.c',
  'newt-frame.c',
  'newt-func.c',
  'newt-list.c',
  'newt-main.c',
  'newt-memory.c',
  'newt-name.c',
  'newt-poly.c',
  'newt-string.c'
]

executable('newt', cfiles, lfiles, pfiles)
