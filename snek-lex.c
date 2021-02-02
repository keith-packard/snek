/*
 * Copyright © 2018 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include "snek.h"

snek_offset_t snek_lex_line = 1;
char *snek_file = "<stdin>";

uint8_t snek_current_indent;
uint8_t snek_lex_indent;
uint8_t snek_ignore_nl;
bool snek_lex_midline;
bool snek_lex_exdent;

#ifndef SNEK_MAX_TOKEN
#define SNEK_MAX_TOKEN	255
#endif

char snek_lex_text[SNEK_MAX_TOKEN + 1];
#if SNEK_MAX_TOKEN > 255
static uint16_t snek_lex_len;
#else
static uint8_t snek_lex_len;
#endif

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

#ifndef TOKEN_INVALID
#define TOKEN_INVALID FIRST_NON_TERMINAL
#endif

static char
lexchar(void)
{
	char ch;

	if (ungetcount)
		ch = ungetbuf[--ungetcount];
	else {
		int c = SNEK_GETC();
		if (c == EOF)
			ch = SNEK_EOF;
		else
			ch = c;
	}
	if (ch == '\n')
		++snek_lex_line;
	return ch;
}

static void
unlexchar(char c)
{
	if (c == '\n')
		--snek_lex_line;
	ungetbuf[ungetcount++] = c;
}

static void __attribute__((noinline))
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

static char
lextoken(void)
{
	char ch = lexchar();
	if (ch != SNEK_EOF)
		if (!add_token(ch))
			return SNEK_EOF;
	return ch;
}

static void
unlextoken(char ch)
{
	unlexchar(ch);
	if (snek_lex_len)
		snek_lex_text[--snek_lex_len] = '\0';
}

static token_t
check_equal(token_t plain_token, snek_op_t op)
{
	char n = lextoken();

	if (n != '=') {
		unlextoken(n);
		RETURN_OP(op, plain_token);
	}
	op += (snek_op_assign_plus - snek_op_plus);
	RETURN_OP(op, ASSIGN);
}

static bool
is_name(char c, bool first)
{
	if (c == '_')
		return true;
	if (c & 0x80)
		return true;
	if (!first) {
		if (c == '.')
			return true;
		if ('0' <= c && c <= '9')
			return true;
	}
	c |= ('a' - 'A');
	if ('a' <= c && c <= 'z')
		return true;
	return false;
}

static int8_t __attribute__ ((noinline))
hex(char c)
{
	if ('0' <= c && c <= '9')
		return c - '0';
	c |= ('a' - 'A');
	if ('a' <= c && c <= 'f')
		return c - ('a' - 10);
	return -1;
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

typedef enum nstate {
	n_int,
	n_frac,
	n_expsign,
	n_exp
} __attribute__((packed)) nstate_t;

typedef enum nclass {
	c_digit,
	c_dot,
	c_e,
	c_sign,
	c_other,
	c_underscore
} __attribute__((packed)) nclass_t;

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
	if (c == '_')
		return c_underscore;
	return c_other;
}

static token_t
number(char c)
{
	nstate_t n = n_int;
	nclass_t t = c_digit;

	start_token();
	for (;;) {
		if (t != c_underscore && !add_token(c))
			RETURN(TOKEN_INVALID);
		c = lexchar();
		t = cclass(c);
		switch (n) {
		case n_int:
			switch (t) {
			case c_digit:
			case c_underscore:
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
	int8_t t;

	start_token();
	for (;;) {
		c = lexchar();
		if (c == q) {
			char *ret = snek_alloc(snek_lex_len + 1);
			if (!ret)
				RETURN(TOKEN_INVALID);
			strcpy(ret, snek_lex_text);
			snek_token_val.string = ret;
			RETURN(STRING);
		}
		if (c == '\n') {
			unlexchar(c);
			RETURN(FIRST_NON_TERMINAL);
		}
		if (c == '\\') {
			c = lexchar();
			switch (c) {
			case 'n':
				c = '\n';
				break;
			case 'r':
				c = '\r';
				break;
			case 't':
				c = '\t';
				break;
			case 'x':
				t = hex(lexchar()) << 4;
				t |= hex(lexchar());
				if (t < 0)
					RETURN(TOKEN_INVALID);
				c = t;
				break;
			default:
				break;
			}
		}
		if (!add_token(c))
			RETURN(TOKEN_INVALID);
	}
}

static token_t
trailing(const char *next, snek_op_t without_op, token_t without, snek_op_t with_op, token_t with)
{
	char c;
	const char *n = next;
	bool space = false;

	/* skip spaces between words */
	while ((c = lexchar()) == ' ')
		space = true;

	/* clean up the token buffer so that errors look good */
	if (space)
		add_token(' ');
	add_token(c);

	/* match trailing word if present */
	for (;;) {
		if (c != *n) {
			unlextoken(c);
			while (n > next)
				unlextoken(*--n);
			if (space)
				unlextoken(' ');
			RETURN_OP(without_op, without);
		}
		if (*++n == '\0') {
			RETURN_OP(with_op, with);
		}
		c = lextoken();
	}
}

static token_t __attribute__((noinline))
snek_lex_close(token_t t)
{
	if (snek_ignore_nl)
		snek_ignore_nl--;
	return t;
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

				if (!snek_interactive && c == '\n') {
					;
				} else if (c == '#') {
					if (!comment())
						RETURN(END);
				} else {
					break;
				}
			}

			snek_lex_midline = true;

			unlexchar(c);

			if (!snek_ignore_nl) {
				if (snek_lex_indent > snek_current_indent) {
					snek_token_val.indent = snek_current_indent;
					snek_current_indent = snek_lex_indent;
					RETURN(INDENT);
				}

				if (snek_lex_indent < snek_current_indent)
					snek_lex_exdent = true;
			}
		}

		/* Generate EXDENT tokens until snek_current_indent is no
		 * bigger than the indent for this line
		 */

		if (snek_lex_exdent && !snek_ignore_nl) {
			if (snek_lex_indent < snek_current_indent) {
				snek_token_val.indent = snek_lex_indent;
				RETURN(EXDENT);
			}
			snek_lex_exdent = false;
		}

		start_token();
		c = lextoken();

		switch (c) {
		case SNEK_EOF:
			RETURN(END);
		case '\n':
			snek_lex_midline = false;
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
			return snek_lex_close(CP);
		case '[':
			++snek_ignore_nl;
			RETURN(OS);
		case ']':
			return snek_lex_close(CS);
#ifndef SNEK_NO_DICT
		case '{':
			++snek_ignore_nl;
			RETURN(OC);
		case '}':
			return snek_lex_close(CC);
#endif
		case '+':
			return check_equal(PLUS, snek_op_plus);
		case '-':
			return check_equal(MINUS, snek_op_minus);
		case '*':
			n = lextoken();
			if (n == '*') {
				return check_equal(POW, snek_op_pow);
			}
			unlextoken(n);
			return check_equal(MULOP, snek_op_times);
		case '/':
			n = lextoken();
			if (n == '/') {
				return check_equal(MULOP, snek_op_div);
			}
			unlextoken(n);
			return check_equal(MULOP, snek_op_divide);
		case '%':
			return check_equal(MULOP, snek_op_mod);
		case '&':
			return check_equal(LAND, snek_op_land);
		case '|':
			return check_equal(LOR, snek_op_lor);
		case '~':
			RETURN_OP(snek_op_lnot, LNOT);
		case '^':
			return check_equal(LXOR, snek_op_lxor);
		case '<':
			n = lextoken();
			if (n == '<') {
				return check_equal(SHIFT, snek_op_lshift);
			}
			if (n == '=') {
				RETURN_OP(snek_op_le, CMPOP);
			}
			unlextoken(n);
			RETURN_OP(snek_op_lt, CMPOP);
		case '=':
			n = lextoken();
			if (n == '=') {
				RETURN_OP(snek_op_eq, CMPOP);
			}
			unlextoken(n);
			RETURN_OP(snek_op_assign, ASSIGN);
		case '>':
			n = lextoken();
			if (n == '>') {
				return check_equal(SHIFT, snek_op_rshift);
			}
			if (n == '=') {
				RETURN_OP(snek_op_ge, CMPOP);
			}
			unlextoken(n);
			RETURN_OP(snek_op_gt, CMPOP);
		case '!':
			n = lextoken();
			if (n == '=') {
				RETURN_OP(snek_op_ne, CMPOP);
			}
			unlextoken(n);
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
			RETURN(TOKEN_INVALID);

		do {
			c = lextoken();
		} while (is_name(c, false));
		unlextoken(c);

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

		snek_token_val.id = id;
		RETURN(NAME);
	}
}
