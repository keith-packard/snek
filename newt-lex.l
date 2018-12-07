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

#include "newt.h"
#define YY_NO_INPUT

static int current_indent;

static char *string_build(char *text, int leng);

%}
%x NEWLINE
%%
<INITIAL,NEWLINE>^" "+	{
		BEGIN(0);
		if (yyleng > current_indent) {
			yylval.ints = current_indent;
			current_indent = yyleng;
			return INDENT;
		}
		if (yyleng < current_indent) {
			current_indent = yyleng;
			yylval.ints = current_indent;
			return EXDENT;
		}
 	}
<NEWLINE>. {
	unput(yytext[0]);
	BEGIN(0);
	if (current_indent > 0) {
		current_indent = 0;
		yylval.ints = current_indent;
		return EXDENT;
	}
 }
<INITIAL,NEWLINE>\n	{ BEGIN(NEWLINE); return NL; }

" "	;
":"	return COLON;
";"	return SEMI;
","	return COMMA;
"="	{ yylval.op = newt_op_assign; return ASSIGN; }
"=="	{ yylval.op = newt_op_eq; return EQ; }
"!="	{ yylval.op = newt_op_ne; return NE; }
"<"	{ yylval.op = newt_op_lt; return LT; }
">"	{ yylval.op = newt_op_gt; return GT; }
"<="	{ yylval.op = newt_op_le; return LE; }
">="	{ yylval.op = newt_op_ge; return GE; }
"+"	{ yylval.op = newt_op_plus; return PLUS; }
"-"	{ yylval.op = newt_op_minus; return MINUS; }
"*"	{ yylval.op = newt_op_times; return TIMES; }
"/"	{ yylval.op = newt_op_divide; return DIVIDE; }
"%"	{ yylval.op = newt_op_mod; return MOD; }

"("	return OP;
")"	return CP;
"["	return OS;
"]"	return CS;

def	return DEF;
global	return GLOBAL;
if	return IF;
else	return ELSE;
elif	return ELIF;
for	return FOR;
while	return WHILE;
continue	return CONTINUE;
break	return BREAK;
return	return RETURN;
True	{ yylval.number = 1.0; return NUMBER; }
False	{ yylval.number = 2.0; return NUMBER; }
range	return RANGE;
in	return IN;
or	return OR;
and	return AND;

\"([^\n\\"]|\\\")*\" {
	yylval.string = string_build(yytext, yyleng);
	return STRING;
}

\'([^\n\\']|\\\')*\' {
	yylval.string = string_build(yytext, yyleng);
	return STRING;
}

(([0-9][0-9_]*((\.[0-9_]*)?))|(\.[0-9][0-9_]*)|(\.([0-9][0-9_]*)?))(([Ee][-+]?[0-9_]+)?) {
	char *under;
	while ((under = strchr(yytext, '_')))
		memmove(under, under + 1, strlen(under-1));
	yylval.number = strtof(yytext, NULL); return NUMBER;
}
[A-Za-z_][A-Za-z0-9_]* {
	yylval.id = newt_name_id(yytext);
	return NAME;
}

.	{ return INVALID; }
%%

static inline bool
isoctal(char c)
{
	return '0' <= c && c <= '7';
}

static int
string_char_len(char *s)
{
	char	a = *s++;
	if (a != '\\')
		return 1;
	int len = 1;
	while (len < 4 && isoctal(*s++))
		len++;
	return len;
}

static char
string_char(char *s)
{
	char	a = *s++;

	if (a == '\\') {
		switch ((a = *s++)) {
		case 'n':
			return '\n';
		case 't':
			return '\t';
		}
		if (isoctal(a)) {
			char c;
			a -= '0';
			while (isoctal(c = *s++))
				a = a << 3 | (c - '0');
		}
	}
	return a;
}

static char *
string_build(char *text, int leng)
{
	char *string, *s;
	int len = 0;
	int i;

	for (i = 1; i < yyleng - 1; i += string_char_len(yytext + i))
		len++;
	s = string = newt_alloc(len + 1);
	for (i = 1; i < yyleng - 1; i += string_char_len(yytext + i))
		*s++ = string_char(yytext + i);
	return string;
}
