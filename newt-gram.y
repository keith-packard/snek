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
	newt_offset_t	offset;
	char		*name;
	float		number;
}

%type   <ints>		block
%type	<offset>	expr

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
		{
			newt_code_t *code = newt_code_finish();
			newt_poly_t p = newt_code_run(code);
			printf("value: %g\n", newt_poly_to_float(p));
		}
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
		{ $$ = $2; }
	| expr PLUS expr
		{
		bin_op:
			newt_code_set_push($1);
			$$ = newt_code_add_op($2);
		}
	| expr MINUS expr
		{ goto bin_op; }
	| expr TIMES expr
		{ goto bin_op; }
	| expr DIVIDE expr
		{ goto bin_op; }
	| expr MOD expr
		{ goto bin_op; }
	| MINUS expr %prec UMINUS
		{ $$ = newt_code_add_op(newt_op_uminus); }
	| expr EQ expr
		{ goto bin_op; }
	| expr NE expr
		{ goto bin_op; }
	| expr LT expr
		{ goto bin_op; }
	| expr GT expr
		{ goto bin_op; }
	| expr LE expr
		{ goto bin_op; }
	| expr GE expr
		{ goto bin_op; }
	| NAME
		{
			printf("name %s\n", $1);
		}
	| NUMBER
		{ $$ = newt_code_add_number($1); }
	;
%%
