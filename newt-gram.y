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

#define MAX_FORMALS	10

static int nformal;
static newt_id_t formals[MAX_FORMALS];

%}
%union {
	bool		bools;
	int		ints;
	newt_op_t	op;
	newt_offset_t	offset;
	newt_id_t	id;
	float		number;
	char		*string;
}

%type	<bools>		stat
%type	<offset>	opt_expr opt_stride branch_false branch_true
%type	<offset>	expr if elif else
%type	<offset>	while mark
%type	<ints>		opt_actuals actuals

%token	<ints>		INDENT EXDENT
%token  <number> 	NUMBER
%token	<string>	STRING
%token  <id>		NAME
%token			INVALID
%token			NL

%token			COLON COMMA SEMI

%right	<op>		ASSIGN
%left	<op>		OR
%left	<op>		AND
%right	<op>		NOT
%left	<op>		EQ NE LT GT LE GE IS IS_NOT IN NOT_IN
%left	<op>		LOR
%left	<op>		LAND
%left	<op>		LXOR
%left	<op>		LSHIFT RSHIFT
%left	<op>		PLUS MINUS
%left	<op>		TIMES DIVIDE DIV MOD
%right	<op>		UMINUS LNOT
%right	<op>		POW
%left	<op>		OP CP OS CS

%token			DEF GLOBAL
%token			IF ELSE ELIF
%token			FOR WHILE
%token			RETURN CONTINUE BREAK
%token			RANGE

%%
file	: file pcommand
	|
	;

pcommand: stat
		{
			newt_code_t *code = newt_code_finish();
			newt_poly_t p = newt_code_run(code);
			if ($1) {
				newt_poly_print(p);
				printf("\n");
			}
		}
	| def
        | error NL
		{ yyerrok; }
	;
def	: DEF { nformal = 0; } NAME OP opt_params CP mark COLON suite
		{
			newt_code_patch_forward($7, newt_forward_return, newt_code_current());
			newt_code_t	*code = newt_code_finish();
			newt_func_t	*func = newt_func_alloc(code, nformal, formals);
			newt_poly_t	poly = newt_func_to_poly(func);

			newt_poly_stash(poly);
			newt_variable_t *var = newt_frame_lookup($3, true);
			poly = newt_poly_fetch();
			if (var)
				var->value = poly;
		}
	;
opt_params: params
	|
	;
params	: params COMMA param
	| param
	;
param	: NAME
		{
			formals[nformal++] = $1;
		}
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
	| expr OS expr CS ASSIGN expr
		{
			newt_code_set_push($1);
			newt_code_set_push($3);
			newt_code_add_op_id($5, NEWT_ID_NONE);
		}
	| RETURN expr
		{
			newt_code_add_forward(newt_forward_return);
		}
	| BREAK
		{
			newt_code_add_forward(newt_forward_break);
		}
	| CONTINUE
		{
			newt_code_add_forward(newt_forward_continue);
		}
	;
compound_stat: if_stat elif_stats else_stat
	| while_stat else_stat
	;
if_stat	: if suite
		{ newt_code_patch_branch($1, newt_code_current()); }
if	: IF expr COLON
		{ $$ = newt_code_add_op_branch(newt_op_branch_false); }
	;
elif_stats	: elif_stats elif_stat
	|
	;
elif_stat: elif suite
		{ newt_code_patch_branch($1, newt_code_current()); }
	;
elif	: ELIF expr COLON
		{ $$ = newt_code_add_op_branch(newt_op_branch_false); }
	;
else_stat: else suite 
		{ newt_code_patch_branch($1, newt_code_current()); }
	|
	;
else	: ELSE COLON
		{ $$ = newt_code_add_op_branch(newt_op_branch); }
	;
while_stat : mark while suite
		{
			newt_offset_t loop = newt_code_add_op_branch(newt_op_branch);
			newt_code_patch_branch($2, newt_code_current());
			newt_code_patch_branch(loop, $1);
			newt_code_patch_forward($2, newt_forward_continue, $1);
			newt_code_patch_forward($2, newt_forward_break, newt_code_current());
		}
	;
while	: WHILE expr COLON
		{ $$ = newt_code_add_op_branch(newt_op_branch_false); }
	;
mark	:
		{ $$ = newt_code_current(); }
	;
suite	: simple_stat
	| NL INDENT stats EXDENT
		{ if ($2 != $4) YYERROR; }
	;
	;
expr	: OP expr CP
		{ $$ = $2; }
	| expr OS expr CS
		{
			newt_code_set_push($1);
			$$ = newt_code_add_op(newt_op_array_fetch);
		}
	| expr OS opt_expr COLON opt_expr opt_stride CS
		{
			bool present = false;
			bool start = false, end = false, stride = false;
			if ($6 != NEWT_OFFSET_NONE) {
				present = true;
				stride = true;
			}
			if ($5 != NEWT_OFFSET_NONE) {
				if (present)
					newt_code_set_push($5);
				present = true;
				end = true;
			}
			if ($3 != NEWT_OFFSET_NONE) {
				if (present)
					newt_code_set_push($3);
				present = true;
				start = true;
			}
			if (present)
				newt_code_set_push($1);
			$$ = newt_code_add_slice(start, end, stride);
		}
	| expr OP opt_actuals CP
		{
			newt_code_set_push($1);
			$$ = newt_code_add_call($3);
		}
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
	| expr DIV expr
		{ goto bin_op; }
	| expr MOD expr
		{ goto bin_op; }
	| expr OR branch_true expr
		{ newt_code_patch_branch($3, newt_code_current()); $$ = $4; }
	| expr AND branch_false expr
		{ newt_code_patch_branch($3, newt_code_current()); $$ = $4; }
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
	| expr IS expr
		{ goto bin_op; }
	| expr IS_NOT expr
		{ goto bin_op; }
	| expr NOT_IN expr
		{ goto bin_op; }
	| expr IN expr
		{ goto bin_op; }
	| expr LOR expr
		{ goto bin_op; }
	| expr LAND expr
		{ goto bin_op; }
	| expr LXOR expr
		{ goto bin_op; }
	| expr LSHIFT expr
		{ goto bin_op; }
	| expr RSHIFT expr
		{ goto bin_op; }
	| NOT expr
		{ unop: $$ = newt_code_add_op($1); }
	| LNOT expr
		{ goto unop; }
	| MINUS expr %prec UMINUS
		{ $$ = newt_code_add_op(newt_op_uminus); }
	| NAME
		{ $$ = newt_code_add_op_id(newt_op_id, $1); }
	| NUMBER
		{ $$ = newt_code_add_number($1); }
	| STRING
		{ $$ = newt_code_add_string($1); }
	| OS actuals CS
		{ $$ = newt_code_add_list($2); }
	| OP actuals CP
		{ $$ = newt_code_add_list($2); }
	;
branch_false:
		{ $$ = newt_code_add_op_branch(newt_op_branch_false); }
	;
branch_true:
		{ $$ = newt_code_add_op_branch(newt_op_branch_true); }
	;
opt_expr: expr
		{ $$ = $1; }
	|
		{ $$ = NEWT_OFFSET_NONE; }
	;
opt_stride: COLON expr
		{ $$ = $2; }
	| COLON
		{ $$ = NEWT_OFFSET_NONE; }
	|
		{ $$ = NEWT_OFFSET_NONE; }
	;
opt_actuals:	actuals
		{ $$ = $1; }
	|
		{ $$ = 0; }
	;
actuals	: actuals COMMA actual
		{ $$ = $1 + 1; }
	| actual
		{ $$ = 1; }
	;
actual	: expr
		{ newt_code_set_push($1); }
	;
%%
