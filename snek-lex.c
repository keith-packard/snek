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

#include "snek.h"

uint8_t snek_current_indent;

char *snek_file;
snek_offset_t snek_line;

uint8_t snek_ignore_nl;

static snek_offset_t snek_lex_line = 1;
static bool snek_lex_midline;
static bool snek_lex_exdent;

uint8_t snek_lex_indent;

#define SNEK_MAX_TOKEN	63

char snek_lex_text[SNEK_MAX_TOKEN + 1];
static uint8_t snek_lex_len;

//#define DEBUG
#ifdef DEBUG
#define RETURN(token) do { printf("token %s\n", #token); return (token); } while (0)
#else
#define RETURN(token) return(token)
#endif

#define RETURN_OP(_op, ret) do { snek_token_val.op = (_op); RETURN (ret); } while(0)

static char ungetbuf[5];
static uint8_t ungetcount;

#ifndef SNEK_GETC
#define SNEK_GETC() getchar()
#endif

#define SNEK_EOF	0x00

static char
lexchar(void)
{
	if (ungetcount)
		return ungetbuf[--ungetcount];

	int c = SNEK_GETC();
	if (c == EOF)
		return SNEK_EOF;
	return (char) c;
}

static void
unlexchar(char c)
{
	ungetbuf[ungetcount++] = c;
}

static token_t
check_equal(token_t plain_token, snek_op_t plain_op, snek_op_t assign_op)
{
	char n = lexchar();

	if (n != '=') {
		unlexchar(n);
		RETURN_OP(plain_op, plain_token);
	}
	RETURN_OP(assign_op, ASSIGN);
	snek_token_val.op = assign_op;
	RETURN(ASSIGN);
}

static bool
is_name(char c, bool first)
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
is_octal(char c)
{
	if ('0' <= c && c <= '7')
		return true;
	return false;
}

static bool
comment(void)
{
	char	c;

	while ((c = lexchar() != '\n'))
		if (c == SNEK_EOF)
			return false;
	return true;
}

static void
start_token(void)
{
	snek_lex_len = 0;
	snek_lex_text[0] = '\0';
}

static bool
add_token(char c)
{
	if (snek_lex_len == SNEK_MAX_TOKEN)
		return false;
	snek_lex_text[snek_lex_len++] = c;
	snek_lex_text[snek_lex_len] = '\0';
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
cclass(char c)
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

static token_t
number(char c)
{
	nstate_t n = n_int;
	nclass_t t;

	start_token();
	for (;;) {
		if (!add_token(c))
			RETURN(TOKEN_NONE);
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
	snek_token_val.number = strtof(snek_lex_text, NULL);
	RETURN(NUMBER);
}

static token_t
string(char q)
{
	char c;
	char t;

	start_token();
	for (;;) {
		c = lexchar();
		if (c == q) {
			char *ret = snek_alloc(snek_lex_len + 1);
			if (!ret)
				RETURN(TOKEN_NONE);
			strcpy(ret, snek_lex_text);
			snek_token_val.string = ret;
			RETURN(STRING);
		}
		if (c == '\\') {
			c = lexchar();
			switch (c) {
			case 'n':
				c = '\n';
				break;
			case 't':
				c = '\t';
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
			RETURN(TOKEN_NONE);
	}
}

static token_t
trailing(char *next, snek_op_t without_op, token_t without, snek_op_t with_op, token_t with)
{
	char c;
	uint8_t len = snek_lex_len;
	char *n = next;

	/* skip spaces between words */
	while ((c = lexchar()) == ' ')
		add_token(c);

	/* match trailing word if present */
	for (;;) {
		if (c != *n) {
			unlexchar(c);
			while (n > next)
				unlexchar(*--n);
			snek_lex_len = len;
			snek_lex_text[len] = '\0';
			RETURN_OP(without_op, without);
		}
		if (*++n == '\0') {
			RETURN_OP(with_op, with);
		}
		c = lexchar();
	}
}

token_t
snek_lex(void)
{
	char c, n;

	for (;;) {
		/* At begining of line, deal with indent changes */
		if (!snek_lex_midline) {

			/* Find a non-comment line */
			for (;;) {
				snek_lex_indent = 0;

				while ((c = lexchar()) == ' ')
					snek_lex_indent++;

				if (c == SNEK_EOF) {
					snek_lex_indent = 0;
					break;
				}

				if (c == '#') {
					if (!comment())
						RETURN(END);
					++snek_lex_line;
				} else {
					break;
				}
			}

			snek_lex_midline = true;

			unlexchar(c);

			if (snek_lex_indent > snek_current_indent) {
				snek_token_val.indent = snek_current_indent;
				snek_current_indent = snek_lex_indent;
				RETURN(INDENT);
			}

			if (snek_lex_indent < snek_current_indent)
				snek_lex_exdent = true;
		}

		/* Generate EXDENT tokens until snek_current_indent is no
		 * bigger than the indent for this line
		 */

		if (snek_lex_exdent) {
			if (snek_lex_indent < snek_current_indent) {
				snek_token_val.indent = snek_lex_indent;
				RETURN(EXDENT);
			}
			snek_lex_exdent = false;
		}

		c = lexchar();

		start_token();
		add_token(c);

		switch (c) {
		case SNEK_EOF:
			RETURN(END);
		case '\n':
			++snek_lex_line;
			snek_lex_midline = false;
			snek_line = snek_lex_line;
			if (snek_ignore_nl)
				continue;
			RETURN(NL);
		case ':':
			RETURN(COLON);
		case ';':
			RETURN(SEMI);
		case ',':
			RETURN(COMMA);
		case '(':
			++snek_ignore_nl;
			RETURN(OP);
		case ')':
			if (snek_ignore_nl)
				--snek_ignore_nl;
			RETURN(CP);
		case '[':
			++snek_ignore_nl;
			RETURN(OS);
		case ']':
			if (snek_ignore_nl)
				--snek_ignore_nl;
			RETURN(CS);
		case '+':
			return check_equal(PLUS, snek_op_plus, snek_op_assign_plus);
		case '-':
			return check_equal(MINUS, snek_op_minus, snek_op_assign_minus);
		case '*':
			n = lexchar();
			if (n == '*') {
				add_token(n);
				return check_equal(POW, snek_op_pow, snek_op_assign_pow);
			}
			unlexchar(n);
			return check_equal(MULOP, snek_op_times, snek_op_assign_times);
		case '/':
			n = lexchar();
			if (n == '/') {
				add_token(n);
				return check_equal(MULOP, snek_op_div, snek_op_assign_div);
			}
			unlexchar(n);
			return check_equal(MULOP, snek_op_divide, snek_op_assign_divide);
		case '%':
			return check_equal(MULOP, snek_op_mod, snek_op_assign_mod);
		case '&':
			return check_equal(LAND, snek_op_land, snek_op_assign_land);
		case '|':
			return check_equal(LOR, snek_op_lor, snek_op_assign_lor);
		case '~':
			RETURN_OP(snek_op_lnot, LNOT);
		case '^':
			return check_equal(LXOR, snek_op_lxor, snek_op_assign_lxor);
		case '<':
			n = lexchar();
			if (n == '<') {
				add_token(n);
				return check_equal(SHIFT, snek_op_lshift, snek_op_assign_lshift);
			}
			if (n == '=') {
				add_token(n);
				RETURN_OP(snek_op_le, CMPOP);
			}
			unlexchar(n);
			RETURN_OP(snek_op_lt, CMPOP);
		case '=':
			n = lexchar();
			if (n == '=') {
				add_token(n);
				RETURN_OP(snek_op_eq, CMPOP);
			}
			unlexchar(n);
			RETURN_OP(snek_op_assign, ASSIGN);
		case '>':
			n = lexchar();
			if (n == '>') {
				add_token(n);
				return check_equal(SHIFT, snek_op_rshift, snek_op_assign_rshift);
			}
			if (n == '=') {
				add_token(n);
				RETURN_OP(snek_op_ge, CMPOP);
			}
			unlexchar(n);
			RETURN_OP(snek_op_gt, CMPOP);
		case '!':
			n = lexchar();
			if (n == '=') {
				add_token(n);
				RETURN_OP(snek_op_ne, CMPOP);
			}
			unlexchar(n);
			break;
		case '"':
		case '\'':
			return string(c);
		case '#':
			if (!comment())
				RETURN(END);
			unlexchar('\n');
			continue;
		case ' ':
			continue;
		}

		if (('0' <= c && c <= '9') || c == '.')
			return number(c);

		if (!is_name(c, true))
			RETURN(TOKEN_NONE);

		start_token();
		do {
			if (!add_token(c))
				RETURN(TOKEN_NONE);
			c = lexchar();
		} while (is_name(c, false));
		unlexchar(c);

		bool keyword;
		snek_id_t id = snek_name_id(snek_lex_text, &keyword);

		if (keyword) {
			snek_token_val.line = snek_lex_line;
			switch (id) {
			case IS:
				return trailing("not", snek_op_is, CMPOP, snek_op_is_not, CMPOP);
			case NOT:
				return trailing("in", snek_op_not, NOT, snek_op_not_in, CMPOP);
			case IN:
				snek_token_val.op = snek_op_in;
				break;
			}
			return id;
		}

		switch (id) {
		case SNEK_BUILTIN_False:
			snek_token_val.number = 0.0f;
			RETURN(NUMBER);
		case SNEK_BUILTIN_True:
			snek_token_val.number = 1.0f;
			RETURN(NUMBER);
		}

		snek_token_val.id = id;
		RETURN(NAME);
	}
}
