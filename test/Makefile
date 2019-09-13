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

SNEK?=$(SNEK_PORTS)/posix/snek

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
	assert-success.py

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
		if python3 $$TEST; then \
			echo "    python pass"; \
		else \
			echo "    ***************** python fail *********************"; \
			exit=1; \
		fi; \
		if $(SNEK) $$TEST; then \
			echo "    snek pass"; \
		else \
			echo "    ***************** snek fail ***********************"; \
			exit=1; \
		fi; \
	done; \
	for TEST in $(FAIL_TESTS); do \
		echo "Running test $$TEST."; \
		python3 $$TEST 2>/dev/null; \
		if [ $$? -eq 1 ]; then \
			echo "    python pass"; \
		else \
			echo "    ***************** python fail *********************"; \
			exit=1; \
		fi; \
		$(SNEK) $$TEST 2>/dev/null; \
		if [ $$? -eq 1 ]; then \
			echo "    snek pass"; \
		else \
			echo "    ***************** snek fail ***********************"; \
			exit=1; \
		fi; \
	done; \
	exit $$exit
