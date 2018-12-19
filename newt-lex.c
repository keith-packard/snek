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

static int newt_lex_line = 1;
static bool newt_lex_newline = true;
static bool newt_lex_exdent = false;
static int newt_lex_indent = 0;

#define NEWT_MAX_TOKEN	63

char yytext[NEWT_MAX_TOKEN + 1];
int yyleng;

//#define DEBUG
#ifdef DEBUG
#define RETURN(token) do { printf("token %s\n", #token); return (token); } while (0)
#else
#define RETURN(token) return(token)
#endif

#define RETURN_OP(_op, ret) do { yylval.op = (_op); RETURN (ret); } while(0)
#define RETURN_INTS(_ints, ret) do { yylval.ints = (_ints); RETURN (ret); } while(0)

static char ungetbuf[5];
static int ungetcount;

#ifndef NEWT_GETC
#define NEWT_GETC() getchar()
#endif

static int
lexchar(void)
{
	if (ungetcount)
		return ((int) ungetbuf[--ungetcount]) & 0xff;
	return NEWT_GETC();
}

static void
unlexchar(int c)
{
	ungetbuf[ungetcount++] = c;
}

static int
check_equal(int plain_token, newt_op_t plain_op, newt_op_t assign_op)
{
	int n = lexchar();

	if (n != '=') {
		unlexchar(n);
		RETURN_OP(plain_op, plain_token);
	}
	RETURN_OP(assign_op, ASSIGN);
	yylval.op = assign_op;
	RETURN(ASSIGN);
}

static bool
is_name(int c, bool first)
{
	if ('A' <= c && c <= 'Z')
		return true;
	if ('a' <= c && c <= 'z')
		return true;
	if (c == '_')
		return true;
	if (!first) {
		if (c == '.')
			return true;
		if ('0' <= c && c <= '9')
			return true;
	}
	return false;
}

static bool
is_octal(int c)
{
	if ('0' <= c && c <= '7')
		return true;
	return false;
}

static bool
comment(void)
{
	int	c;

	while ((c = lexchar() != '\n'))
		if (c == EOF)
			return false;
	++newt_lex_line;
	return true;
}

static void
start_token(void)
{
	yyleng = 0;
	yytext[0] = '\0';
}

static bool
add_token(int c)
{
	if (yyleng == NEWT_MAX_TOKEN)
		return false;
	yytext[yyleng++] = c;
	yytext[yyleng] = '\0';
	return true;
}

typedef enum nstate {
	n_int,
	n_frac,
	n_expsign,
	n_exp
} nstate_t;

typedef enum nclass {
	c_digit,
	c_dot,
	c_e,
	c_sign,
	c_other,
} nclass_t;

static nclass_t
cclass(int c)
{
	if ('0' <= c && c <= '9')
		return c_digit;
	if (c == '.')
		return c_dot;
	if (c == 'e' || c == 'E')
		return c_e;
	if (c == '-' || c == '+')
		return c_sign;
	return c_other;
}

static int
number(int c)
{
	nstate_t n = n_int;
	nclass_t t;

	start_token();
	for (;;) {
		if (!add_token(c))
			RETURN(INVALID);
		c = lexchar();
		t = cclass(c);
		switch (n) {
		case n_int:
			switch (t) {
			case c_digit:
				continue;
			case c_dot:
				n = n_frac;
				continue;
			case c_e:
				n = n_expsign;
				continue;
			default:
				break;
			}
			break;
		case n_frac:
			switch (t) {
			case c_digit:
				continue;
			case c_e:
				n = n_expsign;
				continue;
			default:
				break;
			}
			break;
		case n_expsign:
			switch (t) {
			case c_sign:
				n = n_exp;
				continue;
			case c_digit:
				n = n_exp;
				continue;
			default:
				break;
			}
			break;
		case n_exp:
			switch (t) {
			case c_digit:
				continue;
			default:
				break;
			}
			break;
		}
		break;
	}

	unlexchar(c);
	yylval.number = strtof(yytext, NULL);
	RETURN(NUMBER);
}

static int
string(int q)
{
	int c;
	int t;

	start_token();
	for (;;) {
		c = lexchar();
		if (c == q) {
			char *ret = newt_alloc(yyleng + 1);
			strcpy(ret, yytext);
			yylval.string = ret;
			RETURN(STRING);
		}
		if (c == '\\') {
			c = lexchar();
			switch (c) {
			case 'n':
				c = '\n';
				break;
			case 't':
				c = '\n';
				break;
			default:
				if (is_octal(c)) {
					t = 0;
					do {
						t = t << 3 | (c - '0');
						c = lexchar();
					} while (is_octal(c));
					unlexchar(c);
					c = t;
				} else {
				}
				break;
			}
		}
		if (!add_token(c))
			RETURN(INVALID);
	}
}

static int
trailing(char *next, newt_op_t without_op, int without, newt_op_t with_op, int with)
{
	int c;
	int len = yyleng;
	char *n = next;

	/* skip spaces between words */
	while ((c = getchar()) == ' ')
		add_token(c);

	/* match trailing word if present */
	for (;;) {
		if (c != *n) {
			unlexchar(c);
			while (n > next)
				unlexchar(*--n);
			yyleng = len;
			yytext[len] = '\0';
			RETURN_OP(without_op, without);
		}
		if (*++n == '\0')
			RETURN_OP(with_op, with);
		c = lexchar();
	}
}

int
yylex(void)
{
	int c, n;

	for (;;) {
		/* At begining of line, deal with indent changes */
		if (newt_lex_newline) {

			/* Find a non-blank, non-comment line */
			for (;;) {
				newt_lex_indent = 0;
				while ((c = lexchar()) == ' ')
					newt_lex_indent++;

				if (c == EOF) {
					newt_lex_indent = 0;
					break;
				}

				if (c == '#') {
					if (!comment())
						RETURN(0);
				} else if (c == '\n') {
					++newt_lex_line;
				} else {
					break;
				}
			}

			newt_lex_newline = false;

			if (c != EOF)
				unlexchar(c);

			if (newt_lex_indent > newt_current_indent) {
				yylval.ints = newt_current_indent;
				newt_current_indent = newt_lex_indent;
				RETURN(INDENT);
			}

			if (newt_lex_indent < newt_current_indent)
				newt_lex_exdent = true;
		}

		/* Generate EXDENT tokens until newt_current_indent is no
		 * bigger than the indent for this line
		 */

		if (newt_lex_exdent) {
			if (newt_lex_indent < newt_current_indent) {
				yylval.ints = newt_lex_indent;
				RETURN(EXDENT);
			}
			newt_lex_exdent = false;
		}

		c = lexchar();

		start_token();
		add_token(c);

		switch (c) {
		case EOF:
			RETURN(0);
		case '\n':
			++newt_lex_line;
			newt_lex_newline = true;
			newt_line = newt_lex_line;
			RETURN_INTS(newt_line, NL);
		case ':':
			RETURN(COLON);
		case ';':
			RETURN(SEMI);
		case ',':
			RETURN(COMMA);
		case '(':
			RETURN(OP);
		case ')':
			RETURN(CP);
		case '[':
			RETURN(OS);
		case ']':
			RETURN(CS);
		case '+':
			return check_equal(PLUS, newt_op_plus, newt_op_assign_plus);
		case '-':
			return check_equal(MINUS, newt_op_minus, newt_op_assign_minus);
		case '*':
			n = lexchar();
			if (n == '*') {
				add_token(n);
				return check_equal(POW, newt_op_pow, newt_op_assign_pow);
			}
			unlexchar(n);
			return check_equal(TIMES, newt_op_times, newt_op_assign_times);
		case '/':
			n = lexchar();
			if (n == '/') {
				add_token(n);
				return check_equal(DIV, newt_op_div, newt_op_assign_div);
			}
			unlexchar(n);
			return check_equal(DIVIDE, newt_op_divide, newt_op_assign_divide);
		case '%':
			return check_equal(MOD, newt_op_mod, newt_op_assign_mod);
		case '&':
			return check_equal(LAND, newt_op_land, newt_op_assign_land);
		case '|':
			return check_equal(LOR, newt_op_lor, newt_op_assign_lor);
		case '~':
			RETURN_OP(newt_op_lnot, LNOT);
		case '^':
			return check_equal(LXOR, newt_op_lxor, newt_op_assign_lxor);
		case '<':
			n = lexchar();
			if (n == '<') {
				add_token(n);
				return check_equal(LSHIFT, newt_op_lshift, newt_op_assign_lshift);
			}
			if (n == '=') {
				add_token(n);
				RETURN_OP(newt_op_le, LE);
			}
			unlexchar(n);
			RETURN_OP(newt_op_lt, LT);
		case '=':
			n = lexchar();
			if (n == '=') {
				add_token(n);
				RETURN_OP(newt_op_eq, EQ);
			}
			unlexchar(n);
			RETURN_OP(newt_op_assign, ASSIGN);
		case '>':
			n = lexchar();
			if (n == '>') {
				add_token(n);
				return check_equal(RSHIFT, newt_op_rshift, newt_op_assign_rshift);
			}
			if (n == '=') {
				add_token(n);
				RETURN_OP(newt_op_ge, GE);
			}
			unlexchar(n);
			RETURN_OP(newt_op_gt, GT);
		case '"':
		case '\'':
			return string(c);
		case '#':
			if (!comment())
				RETURN(EOF);
			newt_lex_newline = true;
			continue;
		case ' ':
			continue;
		}

		if (('0' <= c && c <= '9') || c == '.')
			return number(c);

		if (!is_name(c, true))
			RETURN(INVALID);

		start_token();
		do {
			if (!add_token(c))
				RETURN(INVALID);
			c = lexchar();
		} while (is_name(c, false));
		unlexchar(c);

		if (!strcmp(yytext, "True")) {
			yylval.number = 1.0;
			RETURN(NUMBER);
		}

		if (!strcmp(yytext, "False")) {
			yylval.number = 0.0;
			RETURN(NUMBER);
		}

		bool keyword;
		newt_id_t id = newt_name_id(yytext, &keyword);

		if (keyword) {
			yylval.ints = newt_lex_line;
			switch (id) {
			case IS:
				return trailing("not", newt_op_is, IS, newt_op_is_not, IS_NOT);
			case NOT:
				return trailing("in", newt_op_not, NOT, newt_op_not_in, NOT_IN);
			default:
				return id;
			}
		}

		yylval.id = id;
		RETURN(NAME);
	}
}
