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

TESTS = \
	andor.py \
	list.py \
	for-array.py \
	for-range.py \
	for-string.py \
	for-break.py \
	for-nested.py \
	global.py \
	if.py \
	op.py \
	while.py \
	while-break.py \
	while-else.py

check:
	for TEST in $(TESTS); do \
		if ! python3 $$TEST; then \
			echo $$TEST failed python3; \
		fi; \
		if ! ../posix/snek $$TEST; then \
			echo $$TEST failed snek; \
		fi; \
	done
