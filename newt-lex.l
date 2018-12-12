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

int newt_current_indent;
int newt_want_indent;

char *newt_file;
int newt_line;

static char *string_build(char *text, int leng);

//#define DEBUG_LEX
#ifdef DEBUG_LEX
static char tmp_token[8192];
#define RETURN(x) do { memcpy(tmp_token, yytext, yyleng); tmp_token[yyleng] = '\0'; printf("token %-20.20s \"%s\"\n", #x, tmp_token); return x; } while(0)
#else
#define RETURN(x) return(x)
#endif

%}
%x NEWLINE
%%
<NEWLINE>" "+[^ ] {
		if (yytext[yyleng-1] != '#') {
			int amount = yyleng-1;
			if (amount > newt_current_indent) {
				unput(yytext[yyleng-1]);
				BEGIN(0);
				yylval.ints = newt_current_indent;
				newt_current_indent = amount;
				RETURN(INDENT);
			}
			if (amount < newt_current_indent)
			{
				yylval.ints = amount;
				yyless(0);
				RETURN(EXDENT);
			}
		}
		unput(yytext[yyleng-1]);
		BEGIN(0);
 	}
<NEWLINE>(.|\n) {
	if (yytext[yyleng-1] != '#') {
		if (0 < newt_current_indent)
		{
			yylval.ints = 0;
			yyless(0);
			RETURN(EXDENT);
		}
	}
	unput(yytext[yyleng-1]);
	BEGIN(0);
 }
<NEWLINE><<EOF>> {
	if (newt_current_indent)
	{
		yylval.ints = 0;
		RETURN(EXDENT);
	}
	BEGIN(0);
}
<INITIAL>\n	{ BEGIN(NEWLINE); newt_line++; RETURN(NL); }

" "	;
":"	RETURN(COLON);
";"	RETURN(SEMI);
","	RETURN(COMMA);
"="	{ yylval.op = newt_op_assign; RETURN(ASSIGN); }
"+="	{ yylval.op = newt_op_assign_plus; RETURN(ASSIGN); }
"-="	{ yylval.op = newt_op_assign_minus; RETURN(ASSIGN); }
"*="	{ yylval.op = newt_op_assign_times; RETURN(ASSIGN); }
"/="	{ yylval.op = newt_op_assign_divide; RETURN(ASSIGN); }
"//="	{ yylval.op = newt_op_assign_div; RETURN(ASSIGN); }
"%="	{ yylval.op = newt_op_assign_mod; RETURN(ASSIGN); }
"&="	{ yylval.op = newt_op_assign_land; RETURN(ASSIGN); }
"|="	{ yylval.op = newt_op_assign_lor; RETURN(ASSIGN); }
"^="	{ yylval.op = newt_op_assign_lxor; RETURN(ASSIGN); }
"<<="	{ yylval.op = newt_op_assign_lshift; RETURN(ASSIGN); }
">>="	{ yylval.op = newt_op_assign_rshift; RETURN(ASSIGN); }
"**="	{ yylval.op = newt_op_assign_pow; RETURN(ASSIGN); }
"=="	{ yylval.op = newt_op_eq; RETURN(EQ); }
"!="	{ yylval.op = newt_op_ne; RETURN(NE); }
"<"	{ yylval.op = newt_op_lt; RETURN(LT); }
">"	{ yylval.op = newt_op_gt; RETURN(GT); }
"<="	{ yylval.op = newt_op_le; RETURN(LE); }
">="	{ yylval.op = newt_op_ge; RETURN(GE); }
"&"	{ yylval.op = newt_op_land; RETURN(LAND); }
"|"	{ yylval.op = newt_op_land; RETURN(LOR); }
"^"	{ yylval.op = newt_op_land; RETURN(LXOR); }
"<<"	{ yylval.op = newt_op_land; RETURN(LSHIFT); }
">>"	{ yylval.op = newt_op_land; RETURN(RSHIFT); }
"+"	{ yylval.op = newt_op_plus; RETURN(PLUS); }
"-"	{ yylval.op = newt_op_minus; RETURN(MINUS); }
"**"	{ yylval.op = newt_op_pow; RETURN(POW); }
"*"	{ yylval.op = newt_op_times; RETURN(TIMES); }
"/"	{ yylval.op = newt_op_divide; RETURN(DIVIDE); }
"//"	{ yylval.op = newt_op_div; RETURN(DIV); }
"%"	{ yylval.op = newt_op_mod; RETURN(MOD); }
"~"	{ yylval.op = newt_op_lnot; RETURN(LNOT); }

"("	RETURN(OP);
")"	RETURN(CP);
"["	RETURN(OS);
"]"	RETURN(CS);

def	RETURN(DEF);
global	RETURN(GLOBAL);
if	RETURN(IF);
else	RETURN(ELSE);
elif	RETURN(ELIF);
for	RETURN(FOR);
while	RETURN(WHILE);
continue	RETURN(CONTINUE);
break	RETURN(BREAK);
return	RETURN(RETURN);
True	{ yylval.number = 1.0; RETURN(NUMBER); }
False	{ yylval.number = 0.0; RETURN(NUMBER); }
range	RETURN(RANGE);
or	RETURN(OR);
and	RETURN(AND);
is	{ yylval.op = newt_op_is; RETURN(IS); }
is[ \t]+not { yylval.op = newt_op_is_not; RETURN(IS_NOT); }
in	{ yylval.op = newt_op_in; RETURN(IN); }
not[ \t]+in { yylval.op = newt_op_not_in; RETURN(NOT_IN); }
not	{ yylval.op = newt_op_not; RETURN(NOT); }

\"([^\n\"]|\\.)*\" {
	yylval.string = string_build(yytext, yyleng);
	RETURN(STRING);
}

\'([^\n\\']|\\\')*\' {
	yylval.string = string_build(yytext, yyleng);
	RETURN(STRING);
}

(([0-9][0-9_]*((\.[0-9_]*)?))|(\.[0-9][0-9_]*)|(\.([0-9][0-9_]*)?))(([Ee][-+]?[0-9_]+)?) {
	char *under;
	while ((under = strchr(yytext, '_')))
		memmove(under, under + 1, strlen(under-1));
	yylval.number = strtof(yytext, NULL); RETURN(NUMBER);
}
[A-Za-z_][A-Za-z0-9_]* {
	yylval.id = newt_name_id(yytext);
	RETURN(NAME);
}

"#"	{
		int c;
		while ((c = input()) != EOF)
			if (c == '\n') {
				++newt_line;
				break;
			}
	}
.	{ RETURN(INVALID); }
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
	do {
		len++;
	} while (len < 4 && isoctal(*s++));
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
