#
# Copyright © 2019 Keith Packard <keithp@keithp.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
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

import math

def fail(which):
    print("fail %s" % which)
    exit(1)

#
# Number-theoretic and representation functions
#
if not math.ceil(0.5) == 1: fail("math.ceil(0.5) == 1")
if not math.ceil(-0.5) == 0: fail("math.ceil(-0.5) == 0")
if not math.copysign(1.1, -2) == -1.1: fail("math.copysign(1.1, -2) == -1.1")
if not math.copysign(-1.1, -2) == -1.1: fail("math.copysign(-1.1, -2) == -1.1")
if not math.copysign(1.1, 2) == 1.1: fail("math.copysign(1.1, 2) == 1.1")
if not math.copysign(-1.1, 2) == 1.1: fail("math.copysign(-1.1, 2) == 1.1")
if not math.fabs(-1.1) == 1.1: fail("math.fabs(-1.1) == 1.1")
if not math.fabs(1.1) == 1.1: fail("math.fabs(1.1) == 1.1")
if not math.factorial(9) == 362880: fail("math.factorial(9) == 362880")
if not math.floor(0.5) == 0: fail("math.floor(0.5) == 0")
if not math.floor(-0.5) == -1: fail("math.floor(-0.5) == -1")
if not math.frexp(12.5) == (0.78125, 4): fail("math.frexp(12.5) == (0.78125, 4)")
if not math.isclose(math.fsum([0.001] * 1000), 1): fail("math.isclose(math.fsum([0.01] * 100), 1)")
if not math.gcd(21,14) == 7: fail("math.gcd(21,14) == 7")
big = 10.1
for i in range(40):
   big *= big 
if not math.isclose(1,1): fail("math.isclose(1,1)")
if not not math.isclose(1.1,1): fail("not math.isclose(1.1,1)")
if not math.isclose(1.1,1,rel_tol=.2): fail("math.isclose(1.1,1,rel_tol=.2)")
if not math.isfinite(math.factorial(33)): fail("math.isfinite(math.factorial(33))")
if not not math.isfinite(big): fail("not math.isfinite(big)")
if not not math.isfinite(math.inf): fail("not math.isfinite(math.inf)")
if not math.isinf(math.inf): fail("math.isinf(math.inf)")
if not math.isinf(big): fail("math.isinf(math.inf)")
if not not math.isinf(math.factorial(33)): fail("not math.isinf(math.factorial(33))")
if not math.isnan(math.nan): fail("math.isnan(math.nan)")
if not math.isnan(math.inf - math.inf): fail("math.isnan(math.inf - math.inf)")
if not not math.isnan(big): fail("not math.isnan(math.inf)")
if not not math.isnan(1.0): fail("not math.isnan(1.0)")
if not math.ldexp(1,2) == 4: fail("math.ldexp(1,2) == 4")
if not math.ldexp(1,-2) == 1/4: fail("math.ldexp(1,-2) == 1/4")
if not math.modf(1.25) == (0.25, 1.0): fail("math.modf(1.25) == (0.25, 1.0)")
if not math.remainder(5,2) == 1: fail("math.remainder(5,2) == 1")
if not math.remainder(2.25,.5) == .25: fail("math.remainder(2.25,.5) == .25")
if not math.trunc(3.1) == 3: fail("math.trunc(3.1) == 3")
if not math.trunc(-3.1) == -3: fail("math.trunc(-3.1) == -3")
if not round(3.4) == 3: fail("round(3.4) == 3")
if not round(3.6) == 4: fail("round(3.6) == 4")
if not round(-3.4) == -3: fail("round(-3.4) == -3")
if not round(-3.6) == -4: fail("round(-3.6) == -4")
#
# Power and logarithmic functions
#
if not math.exp(0) == 1: fail("math.exp(0) == 1")
if not math.isclose(math.exp(1), math.e): fail("isclose(math.exp(1), math.e)")
if not math.isclose(math.exp(1) - 1, math.expm1(1)): fail("isclose(math.exp(1) - 1, math.expm1(1))")
if not math.isclose(math.log(1), 0): fail("math.isclose(math.log(1), 0)")
if not math.isclose(math.log(math.e), 1): fail("math.isclose(math.log(math.e), 1)")
if not math.isclose(math.log1p(1),math.log(2)): fail("math.isclose(math.log1p(1),math.log(2))")
if not math.isclose(math.log2(5.5),math.log(5.5)/math.log(2)): fail("math.isclose(math.log2(5.5),math.log(5.5)/math.log(2))")
if not math.isclose(math.log10(5.5),math.log(5.5)/math.log(10)): fail("math.isclose(math.log10(5.5),math.log(5.5)/math.log(10))")
if not math.isclose(math.pow(2.2,3.3),math.exp(math.log(2.2) * 3.3)): fail("math.isclose(pow(2.2,3.3),math.exp(math.log(2.2) * 3.3))")
if not math.sqrt(4) == 2: fail("math.sqrt(4) == 2")
if not math.isclose(math.sqrt(2) * math.sqrt(2), 2): fail("isclose(math.sqrt(2) * math.sqrt(2), 2)")
#
# Trigonometric functions
#
if not math.isclose(math.acos(0), math.pi/2): fail("math.isclose(math.acos(0), math.pi/2)")
if not math.isclose(math.acos(1), 0): fail("math.isclose(math.acos(1), 0)")
if not math.isclose(math.asin(0), 0): fail("math.isclose(math.asin(0), 0)")
if not math.isclose(math.asin(1), math.pi/2): fail("math.isclose(math.asin(1), math.pi/2)")
if not math.isclose(math.atan(0), 0): fail("math.isclose(math.atan(0), 0)")
if not math.isclose(math.atan(1), math.pi/4): fail("math.isclose(math.atan(1), math.pi/4)")
if not math.isclose(math.atan2(0,1), 0): fail("math.isclose(math.atan2(0,1), 0)")
if not math.isclose(math.atan2(2,2), math.pi/4): fail("math.isclose(math.atan2(2,2), math.pi/4)")

if not math.isclose(math.cos(math.pi/2), 0,abs_tol=1e-6): fail("math.isclose(math.cos(0), math.pi/2)")
if not math.isclose(math.cos(0), 1): fail("math.isclose(math.cos(1), 0)")
if not math.isclose(math.hypot(3,4), 5): fail("math.isclose(math.hypot(3,4), 5)")
if not math.isclose(math.sin(0), 0): fail("math.isclose(math.sin(0), 0)")
if not math.isclose(math.sin(math.pi/2), 1): fail("math.isclose(math.sin(1), math.pi/2)")
if not math.isclose(math.tan(0), 0): fail("math.isclose(math.tan(0), 0)")
if not math.isclose(math.tan(math.pi/4), 1): fail("math.isclose(math.tan(1), math.pi/4)")
#
# Angular conversion
#
if not math.isclose(math.degrees(0),0): fail("math.isclose(math.degrees(0),0)")
if not math.isclose(math.degrees(math.pi),180): fail("math.isclose(math.degrees(math.pi),180)")
if not math.isclose(math.radians(0),0): fail("math.isclose(math.radians(0),0)")
if not math.isclose(math.radians(180), math.pi): fail("math.isclose(math.radians(180), math.pi)")
#
# Hyperbolic functions
#
if not math.isclose(math.sinh(1), (math.e - 1/math.e) / 2): fail("math.isclose(math.sinh(1), (math.e - 1/math.e) / 2)")
if not math.isclose(math.cosh(1), (math.e + 1/math.e) / 2): fail("math.isclose(math.cosh(1), (math.e + 1/math.e) / 2)")
if not math.isclose(math.tanh(1), math.sinh(1)/math.cosh(1)): fail("math.isclose(math.tanh(1), math.sinh(1)/math.cosh(1))")
if not math.isclose(math.asinh(math.sinh(1)), 1): fail("math.isclose(math.asinh(math.sinh(1)), 1)")
if not math.isclose(math.acosh(math.cosh(1)), 1): fail("math.isclose(math.acosh(math.cosh(1)), 1)")
if not math.isclose(math.atanh(math.tanh(1)), 1): fail("math.isclose(math.atanh(math.tanh(1)), 1)")
#
# Special functions
#
if not math.isclose(math.erf(1), 0.8427007929497149): fail("math.isclose(math.erf(1), 0.8427007929497149)")
if not math.isclose(math.erfc(1), 1-math.erf(1)): fail("math.isclose(math.erfc(1), 1-math.erf(1))")
if not math.isclose(math.gamma(10), math.factorial(9)): fail("math.isclose(math.gamma(10), math.factorial(9))")
if not math.isclose(math.lgamma(10), math.log(math.factorial(9))): fail("math.isclose(math.lgamma(10), math.log(math.factorial(9)))")
#
# Constants
#
if not math.isclose(math.pi * 2, math.tau): fail("math.isclose(math.pi * 2, math.tau)")
