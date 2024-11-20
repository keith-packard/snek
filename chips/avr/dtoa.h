/* Copyright © 2018, Keith Packard
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

#ifndef	_DTOA_H_
#define	_DTOA_H_

#define	DTOA_MINUS	1
#define	DTOA_ZERO	2
#define	DTOA_INF	4
#define	DTOA_NAN	8

#define DTOA_MAX_DIG            17
#define DTOA_MAX_10_EXP         308
#define DTOA_MIN_10_EXP         (-307)
#define DTOA_SCALE_UP_NUM       9
#define DTOA_ROUND_NUM          (DTOA_MAX_DIG + 1)
#define DTOA_MAX_EXP            1024

#define FTOA_MAX_10_EXP         38
#define FTOA_MIN_10_EXP         (-37)
#define FTOA_MAX_DIG	        9
#define FTOA_SCALE_UP_NUM       6
#define FTOA_ROUND_NUM          (FTOA_MAX_DIG + 1)

#define DTOA_DIGITS     FTOA_MAX_DIG

struct dtoa {
    int32_t     exp;
    uint8_t     flags;
    char        digits[DTOA_DIGITS];
};

#endif	/* !_DTOA_H_ */
