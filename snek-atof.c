/* Copyright (c) 2002-2005  Michael Stumpf  <mistumpf@de.pepperl-fuchs.com>
   Copyright (c) 2006,2008  Dmitry Xmelkov

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE. */


#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>		/* INFINITY, NAN		*/
#include <stdlib.h>

#ifdef AVR
#define float double
#include <avr/pgmspace.h>
#else
#define __flash
#define pgm_read_dword(x)	(*x)
#endif
#include <snek.h>

/* Only GCC 4.2 calls the library function to convert an unsigned long
   to float.  Other GCC-es (including 4.3) use a signed long to float
   conversion along with a large inline code to correct the result.	*/
extern float __floatunsisf (unsigned long);

static const __flash float pwr_p10 [6] = {
    1e+32, 1e+16, 1e+8, 1e+4, 1e+2, 1e+1,
};
static const __flash float pwr_m10 [6] = {
    1e-32, 1e-16, 1e-8, 1e-4, 1e-2, 1e-1,
};

/**  The atof() function converts the initial portion of the string pointed
     to by \a nptr to float representation.

     The expected form of the string is an optional plus ( \c '+' ) or minus
     sign ( \c '-' ) followed by a sequence of digits optionally containing
     a decimal-point character, optionally followed by an exponent.  An
     exponent consists of an \c 'E' or \c 'e', followed by an optional plus
     or minus sign, followed by a sequence of digits.

     Leading white-space characters in the string are not skipped.

     The atof() function returns the converted value, if any.

     If no conversion is performed, zero is returned and the value of
     \a nptr is stored in the location referenced by \a endptr.

     If the correct value would cause overflow, plus or minus \c INFINITY is
     returned (according to the sign of the value), and \c ERANGE is stored
     in \c errno.  If the correct value would cause underflow, zero is
     returned and \c ERANGE is stored in \c errno.
 */

#define CASE_CONVERT    ('a' - 'A')
#define TOLOWER(c)        ((c) | CASE_CONVERT)

float
atoff (const char * nptr)
{
    union {
	unsigned long u32;
	float flt;
    } x;
    unsigned char c;
    int exp;

    unsigned char flag;
#define FL_ANY	    0x01	/* any digit was readed	*/
#define FL_OVFL	    0x02	/* overflow was		*/
#define FL_DOT	    0x04	/* decimal '.' was	*/
#define FL_MEXP	    0x08	/* exponent 'e' is neg.	*/

    c = *nptr++;

#if defined(SNEK_BUILTIN_float)
    if (TOLOWER(c) == 'n')
	    return NAN;
    if (TOLOWER(c) == 'i')
	    return INFINITY;
#endif

    flag = 0;

    x.u32 = 0;
    exp = 0;
    while (1) {

	c -= '0';

	if (c <= 9) {
	    flag |= FL_ANY;
	    if (flag & FL_OVFL) {
		if (!(flag & FL_DOT))
		    exp += 1;
	    } else {
		if (flag & FL_DOT)
		    exp -= 1;
		x.u32 = x.u32 * 10 + c;
		if (x.u32 >= (ULONG_MAX - 9) / 10)
		    flag |= FL_OVFL;
	    }

	} else if (c == (('.'-'0') & 0xff)  &&  !(flag & FL_DOT)) {
	    flag |= FL_DOT;
	} else {
	    break;
	}
	c = *nptr++;
    }

    if (TOLOWER(c) == 'e' - '0')
    {
	int i;
	c = *nptr++;
	i = 2;
	if (c == '-') {
	    flag |= FL_MEXP;
	    c = *nptr++;
	} else if (c == '+') {
	    c = *nptr++;
	} else {
	    i = 1;
	}
	c -= '0';
	if (c > 9) {
	    nptr -= i;
	} else {
	    i = 0;
	    do {
		if (i < 3200)
		    i = i * 10 + c;
		c = *nptr++ - '0';
	    } while (c <= 9);
	    if (flag & FL_MEXP)
		i = -i;
	    exp += i;
	}
    }

    x.flt = __floatunsisf (x.u32);		/* manually	*/

    if (x.flt != 0) {
	int pwr;
	const __flash float *fptr;

	if (exp < 0) {
	    fptr = pwr_m10;
	    exp = -exp;
	} else {
	    fptr = pwr_p10;
	}
	for (pwr = 32; pwr; pwr >>= 1) {
	    for (; exp >= pwr; exp -= pwr)
		x.flt *= *fptr;
	    fptr++;
	}
    }

    return x.flt;
}
