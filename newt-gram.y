%{

/*
 * Copyright © 2018 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <stdio.h>
#include "newt.h"

%}
%union {
	int		ints;
	newt_op_t	op;
	char		*name;
	float		number;
}

%type   <ints>		block

%token	<ints>		INDENT EXDENT
%token  <number> 	NUMBER
%token  <name>		NAME
%token			INVALID
%token			NL

%token			COLON COMMA SEMI

%right			ASSIGN
%left	<op>		EQ NE
%left	<op>		LT GT LE GE
%left	<op>		PLUS MINUS
%left	<op>		TIMES DIVIDE MOD
%right			UMINUS
%left			OP CP OS CS

%token			DEF

%%
file	: file pcommand
	|
	;

pcommand: command
        | error NL
		{ yyerrok; }
	;
command	: NAME ASSIGN expr NL
		{ printf ("assignment %s\n", $1); }
	| expr NL
		{ printf ("expr\n"); }
	| NL
		{ printf ("blank line\n"); }
	| block
		{ printf ("block\n"); }
	;
block	: INDENT commands EXDENT
		{ if ($1 != $3) YYERROR; $$ = $1; }
	;
commands: commands command
	| command
	;
expr	: OP expr CP
	| expr PLUS expr
	| expr MINUS expr
	| expr TIMES expr
	| expr DIVIDE expr
	| expr MOD expr
	| MINUS expr %prec UMINUS
	| expr EQ expr
	| expr NE expr
	| expr LT expr
	| expr GT expr
	| expr LE expr
	| expr GE expr
	| NAME
		{
			printf("name %s\n", $1);
		}
	| NUMBER
		{
			printf("number %g\n", $1);
		}
	;
%%
