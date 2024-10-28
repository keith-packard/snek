#
# Copyright © 2019 Keith Packard <keithp@keithp.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
#
# Test math builtins
#

from math import *


def fail(which):
    print("fail %s" % which)
    exit(1)


#
# Number-theoretic and representation functions
#
if not ceil(0.5) == 1:
    fail("ceil(0.5) == 1")
if not ceil(-0.5) == 0:
    fail("ceil(-0.5) == 0")
if not copysign(1.1, -2) == -1.1:
    fail("copysign(1.1, -2) == -1.1")
if not copysign(-1.1, -2) == -1.1:
    fail("copysign(-1.1, -2) == -1.1")
if not copysign(1.1, 2) == 1.1:
    fail("copysign(1.1, 2) == 1.1")
if not copysign(-1.1, 2) == 1.1:
    fail("copysign(-1.1, 2) == 1.1")
if not fabs(-1.1) == 1.1:
    fail("fabs(-1.1) == 1.1")
if not fabs(1.1) == 1.1:
    fail("fabs(1.1) == 1.1")
if not factorial(9) == 362880:
    fail("factorial(9) == 362880")
if not floor(0.5) == 0:
    fail("floor(0.5) == 0")
if not floor(-0.5) == -1:
    fail("floor(-0.5) == -1")
if not frexp(12.5) == (0.78125, 4):
    fail("frexp(12.5) == (0.78125, 4)")
if not isclose(fsum([0.001] * 1000), 1):
    fail("isclose(fsum([0.01] * 100), 1)")
if not gcd(21, 14) == 7:
    fail("gcd(21,14) == 7")
big = 10.1
for i in range(40):
    big *= big
if not isclose(1, 1):
    fail("isclose(1,1)")
if not not isclose(1.1, 1):
    fail("not isclose(1.1,1)")
if not isclose(1.1, 1, rel_tol=0.2):
    fail("isclose(1.1,1,rel_tol=.2)")
if not isfinite(factorial(33)):
    fail("isfinite(factorial(33))")
if not not isfinite(big):
    fail("not isfinite(big)")
if not not isfinite(inf):
    fail("not isfinite(inf)")
if not isinf(inf):
    fail("isinf(inf)")
if not isinf(big):
    fail("isinf(inf)")
if not not isinf(factorial(33)):
    fail("not isinf(factorial(33))")
if not isnan(nan):
    fail("isnan(nan)")
if not isnan(inf - inf):
    fail("isnan(inf - inf)")
if not not isnan(big):
    fail("not isnan(inf)")
if not not isnan(1.0):
    fail("not isnan(1.0)")
if not ldexp(1, 2) == 4:
    fail("ldexp(1,2) == 4")
if not ldexp(1, -2) == 1 / 4:
    fail("ldexp(1,-2) == 1/4")
if not modf(1.25) == (0.25, 1.0):
    fail("modf(1.25) == (0.25, 1.0)")
if not remainder(5, 2) == 1:
    fail("remainder(5,2) == 1")
if not remainder(2.25, 0.5) == 0.25:
    fail("remainder(2.25,.5) == .25")
if not trunc(3.1) == 3:
    fail("trunc(3.1) == 3")
if not trunc(-3.1) == -3:
    fail("trunc(-3.1) == -3")
if not round(3.4) == 3:
    fail("round(3.4) == 3")
if not round(3.6) == 4:
    fail("round(3.6) == 4")
if not round(-3.4) == -3:
    fail("round(-3.4) == -3")
if not round(-3.6) == -4:
    fail("round(-3.6) == -4")
#
# Power and logarithmic functions
#
if not exp(0) == 1:
    fail("exp(0) == 1")
if not isclose(exp(1), e):
    fail("isclose(exp(1), e)")
if not isclose(exp(1) - 1, expm1(1)):
    fail("isclose(exp(1) - 1, expm1(1))")
if not isclose(log(1), 0):
    fail("isclose(log(1), 0)")
if not isclose(log(e), 1):
    fail("isclose(log(e), 1)")
if not isclose(log1p(1), log(2)):
    fail("isclose(log1p(1),log(2))")
if not isclose(log2(5.5), log(5.5) / log(2)):
    fail("isclose(log2(5.5),log(5.5)/log(2))")
if not isclose(log10(5.5), log(5.5) / log(10)):
    fail("isclose(log10(5.5),log(5.5)/log(10))")
if not isclose(pow(2.2, 3.3), exp(log(2.2) * 3.3)):
    fail("isclose(pow(2.2,3.3),exp(log(2.2) * 3.3))")
if not sqrt(4) == 2:
    fail("sqrt(4) == 2")
if not isclose(sqrt(2) * sqrt(2), 2):
    fail("isclose(sqrt(2) * sqrt(2), 2)")
#
# Trigonometric functions
#
if not isclose(acos(0), pi / 2):
    fail("isclose(acos(0), pi/2)")
if not isclose(acos(1), 0):
    fail("isclose(acos(1), 0)")
if not isclose(asin(0), 0):
    fail("isclose(asin(0), 0)")
if not isclose(asin(1), pi / 2):
    fail("isclose(asin(1), pi/2)")
if not isclose(atan(0), 0):
    fail("isclose(atan(0), 0)")
if not isclose(atan(1), pi / 4):
    fail("isclose(atan(1), pi/4)")
if not isclose(atan2(0, 1), 0):
    fail("isclose(atan2(0,1), 0)")
if not isclose(atan2(2, 2), pi / 4):
    fail("isclose(atan2(2,2), pi/4)")

if not isclose(cos(pi / 2), 0, abs_tol=1e-6):
    fail("isclose(cos(0), pi/2)")
if not isclose(cos(0), 1):
    fail("isclose(cos(1), 0)")
if not isclose(hypot(3, 4), 5):
    fail("isclose(hypot(3,4), 5)")
if not isclose(sin(0), 0):
    fail("isclose(sin(0), 0)")
if not isclose(sin(pi / 2), 1):
    fail("isclose(sin(1), pi/2)")
if not isclose(tan(0), 0):
    fail("isclose(tan(0), 0)")
if not isclose(tan(pi / 4), 1):
    fail("isclose(tan(1), pi/4)")
#
# Angular conversion
#
if not isclose(degrees(0), 0):
    fail("isclose(degrees(0),0)")
if not isclose(degrees(pi), 180):
    fail("isclose(degrees(pi),180)")
if not isclose(radians(0), 0):
    fail("isclose(radians(0),0)")
if not isclose(radians(180), pi):
    fail("isclose(radians(180), pi)")
#
# Hyperbolic functions
#
if not isclose(sinh(1), (e - 1 / e) / 2):
    fail("isclose(sinh(1), (e - 1/e) / 2)")
if not isclose(cosh(1), (e + 1 / e) / 2):
    fail("isclose(cosh(1), (e + 1/e) / 2)")
if not isclose(tanh(1), sinh(1) / cosh(1)):
    fail("isclose(tanh(1), sinh(1)/cosh(1))")
if not isclose(asinh(sinh(1)), 1):
    fail("isclose(asinh(sinh(1)), 1)")
if not isclose(acosh(cosh(1)), 1):
    fail("isclose(acosh(cosh(1)), 1)")
if not isclose(atanh(tanh(1)), 1):
    fail("isclose(atanh(tanh(1)), 1)")
#
# Special functions
#
if not isclose(erf(1), 0.8427007929497149):
    fail("isclose(erf(1), 0.8427007929497149)")
if not isclose(erfc(1), 1 - erf(1)):
    fail("isclose(erfc(1), 1-erf(1))")
if not isclose(gamma(10), factorial(9)):
    fail("isclose(gamma(10), factorial(9))")
if not isclose(lgamma(10), log(factorial(9))):
    fail("isclose(lgamma(10), log(factorial(9)))")
#
# Constants
#
if not isclose(pi * 2, tau):
    fail("isclose(pi * 2, tau)")
