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

SNEK_NO_BUILD_TARGETS = 1
SNEK_ROOT = .

include snek-install.defs

SNEK_OTHEROS_DIR=$(SNEK_HOSTS)/linux $(SNEK_HOSTS)/windows $(SNEK_HOSTS)/macosx $(SNEK_HOSTS)/zip

SUBDIRS = snekde doc examples

SNEKS = $(SNEK_PORTS)/posix/snek $(FIRMWARE)

all: $(SNEKS)
	+for dir in $(SUBDIRS); do (cd $$dir && make PREFIX=$(PREFIX) DESTDIR=$(DESTDIR)) || exit 1; done

$(SNEKS): FORCE
	+cd `dirname $@` && make PREFIX=$(PREFIX) DESTDIR=$(DESTDIR)

FORCE:

check: all
	+cd test && make $@

black:
	+black --check --exclude 'fail-syntax-.*\.py|.*/hosts/.*.py|ubaboot.py' .

black-reformat:
	+black --exclude 'fail-syntax-.*\.py|.*/hosts/.*.py|ubaboot.py' .

install: all
	+for dir in $(SUBDIRS); do (cd $$dir && make PREFIX=$(PREFIX) DESTDIR=$(DESTDIR) $@) || exit 1; done
	+for snek in $(SNEKS); do (cd `dirname $$snek` && make PREFIX=$(PREFIX) DESTDIR=$(DESTDIR) $@) || exit 1; done

upload: otheros
	+cd doc && make upload
	+for otheros in $(SNEK_OTHEROS_DIR); do (cd "$$otheros" && make upload); done

install-otheros: otheros install
	+for otheros in $(SNEK_OTHEROS_DIR); do (cd "$$otheros" && make install-otheros); done

otheros: all
	+for otheros in $(SNEK_OTHEROS_DIR); do (cd "$$otheros" && make); done

snek-mu.py:
	find . -name '*.builtin' -print0 | xargs -0 python3 ./snek-builtin.py --mu -o $@

docker:
	docker build -t phsilva/snek .

clean:
	+for dir in $(SUBDIRS); do (cd $$dir && make PREFIX=$(PREFIX) DESTDIR=$(DESTDIR) $@); done
	+for snek in $(SNEKS); do (cd `dirname $$snek` && make PREFIX=$(PREFIX) DESTDIR=$(DESTDIR) $@); done
