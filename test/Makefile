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

SNEK_ROOT=..

include $(SNEK_ROOT)/snek.defs

PYTHON3?=python3
SNEK_NATIVE?=$(SNEK_PORTS)/posix/snek
SNEK_RISCV?=$(SNEK_PORTS)/qemu-riscv/snek-riscv
SNEK_ARM?=$(SNEK_PORTS)/qemu-arm/snek-arm

LANGS=python3 $(SNEK_NATIVE) $(SNEK_ARM) $(SNEK_RISCV)

SUCCESS_TESTS = \
	pass-andor.py \
	pass-list.py \
	pass-dict.py \
	pass-equal_is.py \
	pass-float.py \
	pass-for-array.py \
	pass-for-range.py \
	pass-for-string.py \
	pass-for-break.py \
	pass-for-nested.py \
	pass-global.py \
	pass-if.py \
	pass-int.py \
	pass-math.py \
	pass-nan.py \
	pass-op.py \
	pass-random.py \
	pass-range.py \
	pass-slice.py \
	pass-while.py \
	pass-while-break.py \
	pass-while-else.py \
	pass-scoping-global.py \
	pass-scoping-local.py \
	pass-none.py \
	pass-args.py \
	pass-assert-success.py \
	pass-interpolate-list.py \
	pass-get.py \
	pass-interpolate-str.py \
	pass-trailing-comma.py \
	pass-chain-op.py \
	pass-precedence.py \
	pass-str.py \
	pass-str-op.py

SYNTAX_TESTS = \
	fail-syntax-lex-bang.py \
	fail-syntax-list-named.py \
	fail-syntax-tuple-named.py

FAIL_TESTS = \
	fail-scoping-no-decl.py \
	fail-range-named.py \
	fail-formal-named-first.py \
	fail-actual-named-first.py \
	fail-args-missing.py \
	fail-arg-dup1.py \
	fail-arg-dup2.py \
	fail-arg-unknown.py \
	fail-assert-fail.py \
	fail-interpolate-missing.py \
	fail-interpolate-extra.py \
	fail-interpolate-badformat.py \
	fail-dictionary-mutable.py \
	$(SYNTAX_TESTS)

check:
	@exit=0; \
	for TEST in $(SUCCESS_TESTS); do \
		echo "Running test $$TEST."; \
		for lang in $(LANGS); do \
			baselang=`basename $$lang`; \
			if $$lang $$TEST; then \
				echo "    pass $$baselang"; \
			else \
				echo "    ***************** $$baselang fail *********************"; \
				exit=1;\
			fi; \
		done; \
	done; \
	for TEST in $(FAIL_TESTS); do \
		echo "Running test $$TEST."; \
		for lang in $(LANGS); do \
			baselang=`basename $$lang`; \
			$$lang $$TEST >/dev/null 2>&1; \
			if [ $$? -eq 1 ]; then \
				echo "    pass $$baselang"; \
			else \
				echo "    ***************** $$baselang fail *********************"; \
				exit=1;\
			fi; \
		done; \
	done; \
	exit $$exit
