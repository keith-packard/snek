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
	bool		bools;
	int		ints;
	newt_op_t	op;
	newt_offset_t	offset;
	newt_id_t	id;
	float		number;
}

%type	<bools>		stat
%type	<offset>	expr test if elif else

%token	<ints>		INDENT EXDENT
%token  <number> 	NUMBER
%token  <id>		NAME
%token			INVALID
%token			NL

%token			COLON COMMA SEMI

%right	<op>		ASSIGN
%left	<op>		OR
%left	<op>		AND
%right	<op>		NOT
%left	<op>		EQ NE
%left	<op>		LT GT LE GE
%left	<op>		PLUS MINUS
%left	<op>		TIMES DIVIDE MOD
%right			UMINUS
%left			OP CP OS CS

%token			DEF GLOBAL
%token			IF ELSE ELIF
%token			FOR WHILE CONTINUE BREAK
%token			TRUE FALSE
%token			RANGE IN

%%
file	: file pcommand
	|
	;

pcommand: stat
		{
			newt_code_t *code = newt_code_finish();
			newt_poly_t p = newt_code_run(code);
			if ($1)
				printf(" %g\n", newt_poly_to_float(p));
		}
        | error NL
		{ yyerrok; }
	;
stats	: stats stat
	| stat
	;
stat	: simple_stat
		{ $$ = true; }
	| compound_stat
		{ $$ = false; }
	| NL
		{ $$ = false; }
	;
simple_stat: small_stats NL
	;
small_stats: small_stats SEMI small_stat
	| small_stat
	;
small_stat	: expr
	| NAME ASSIGN expr
		{
			newt_code_add_op_id($2, $1);
		}
	;
compound_stat: if_stat elif_stats else_stat
	;
if_stat	: if suite
		{ newt_code_patch_branch($1, newt_code_current()); }
if	: IF test COLON
		{ $$ = newt_code_add_op_branch(newt_op_if); }
	;
elif_stats	: elif_stats elif_stat
	|
	;
elif_stat: elif suite
		{ newt_code_patch_branch($1, newt_code_current()); }
	;
elif	: ELIF test COLON
		{ $$ = newt_code_add_op_branch(newt_op_if); }
	;
else_stat: else suite 
		{ newt_code_patch_branch($1, newt_code_current()); }
	|
	;
else	: ELSE COLON
		{ $$ = newt_code_add_op_branch(newt_op_branch); }
	;
suite	: simple_stat
	| NL INDENT stats EXDENT
		{ if ($2 != $4) YYERROR; }
	;
test	: test OR test
		{ goto bin_op; }
	| test AND test
		{ goto bin_op; }
	| NOT test
		{ $$ = newt_code_add_op(newt_op_not); }
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
	| NAME
		{ $$ = newt_code_add_op_id(newt_op_id, $1); }
	| NUMBER
		{ $$ = newt_code_add_number($1); }
	;
%%
