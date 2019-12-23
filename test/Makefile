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

LANGS=python3 $(SNEK_NATIVE) $(SNEK_ARM)

SUCCESS_TESTS = \
	andor.py \
	list.py \
	dict.py \
	equal_is.py \
	float.py \
	for-array.py \
	for-range.py \
	for-string.py \
	for-break.py \
	for-nested.py \
	global.py \
	if.py \
	int.py \
	math.py \
	op.py \
	range.py \
	slice.py \
	while.py \
	while-break.py \
	while-else.py \
	scoping-global.py \
	scoping-local.py \
	none.py \
	args.py \
	assert-success.py \
	interpolate-list.py \
	get.py

FAIL_TESTS = \
	scoping-no-decl.py \
	list-named.py \
	tuple-named.py \
	range-named.py \
	formal-named-first.py \
	actual-named-first.py \
	args-missing.py \
	arg-dup1.py \
	arg-dup2.py \
	arg-unknown.py \
	assert-fail.py

check:
	@exit=0; \
	for TEST in $(SUCCESS_TESTS); do \
		echo "Running test $$TEST."; \
		for lang in $(LANGS); do \
			baselang=`basename $$lang`; \
			if $$lang $$TEST; then \
				echo "    pass $$baselang"; \
			else \
				echo "    ***************** $baselang fail *********************"; \
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
				echo "    ***************** $baselang fail *********************"; \
				exit=1;\
			fi; \
		done; \
	done; \
	exit $$exit
