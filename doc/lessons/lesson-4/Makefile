SNEK_NO_BUILD_TARGETS = 1
SNEK_ROOT=../../..

include $(SNEK_ROOT)/snek-install.defs

.SUFFIXES: .adoc .pdf .html

ADOC_FILES=lesson-4-washing-machine.adoc

PDF_FILES=$(ADOC_FILES:.adoc=.pdf)
HTML_FILES=$(ADOC_FILES:.adoc=.html)
CSS_FILES=snek.css  coderay-asciidoctor.css
IMAGE_FILES=\
	washing-machine.jpg \
	21980.dat.png \
	3003.dat.png \
	3022.dat.png \
	3023.dat.png \
	3034.dat.png \
	3039.dat.png \
	3040b.dat.png \
	3460.dat.png \
	3482.dat.png \
	3634.dat.png \
	3660.dat.png \
	3666.dat.png \
	3700.dat.png \
	3701.dat.png \
	3702.dat.png \
	3738.dat.png \
	3830.dat.png \
	3831.dat.png \
	3894.dat.png \
	3895.dat.png \
	41539.dat.png \
	4459.dat.png \
	4519.dat.png \
	5306.dat.png \
	75973.dat.png \
	879.dat.png \
	washing-machine-01.png \
	washing-machine-02.png \
	washing-machine-03.png \
	washing-machine-04.png \
	washing-machine-05.png \
	washing-machine-06.png \
	washing-machine-07.png \
	washing-machine-08.png \
	washing-machine-09.png \
	washing-machine-10.png \
	washing-machine-11.png \
	washing-machine-12.png \
	washing-machine-13.png \
	washing-machine-14.png \
	washing-machine-15.png \
	washing-machine-16.png \
	washing-machine-17.png \
	washing-machine-18.png \
	washing-machine-19.png

INC_FILES=steps.adoc

all: $(HTML_FILES) $(PDF_FILES)

LETTER_ATTRIBUTES=--attribute='pdf-style=snek' 
ATTRIBUTES=--attribute="revdate=$(SNEK_DATE)" --attribute="version=$(SNEK_VERSION)"

.adoc.html:
	asciidoctor -v -B$(SNEK_ROOT) -B. $(ATTRIBUTES) -b html5 $*.adoc

.adoc.pdf:
	asciidoctor-pdf -B$(SNEK_ROOT) -B. $(ATTRIBUTES) $(LETTER_ATTRIBUTES) $*.adoc

$(PDF_FILES): snek-theme.yml $(IMAGE_FILES) $(INC_FILES) snek.svg

DOCFILES=$(PDF_FILES) $(HTML_FILES) $(CSS_FILES) snek.svg $(IMAGE_FILES)

$(HTML_FILES): $(INC_FILES)

install: $(DOCFILES)
	install -d $(DESTDIR)$(DOCDIR)
	for i in $(DOCFILES); do install --mode=644 "$$i" $(DESTDIR)$(DOCDIR) || exit 1; done

upload: $(DOCFILES)
	rsync -avz $(DOCFILES) ../../fonts $(SNEKDOC)/lessons/lesson-4

steps.adoc: $(SNEK_DOC_UTIL)/make-step $(SNEK_DOC_UTIL)/make-steps
	$(SNEK_DOC_UTIL)/make-steps washing-machine-??.html > $@

snek.css: $(SNEK_DOC)/snek.css
	cp $^ $@

snek.svg: $(SNEK_ROOT)/snek.svg
	cp $^ $@

snek-theme.yml: $(SNEK_DOC)/snek-theme.yml
	cp $^ $@

coderay-asciidoctor.css: $(HTML_FILES)

clean::
	rm -f $(PDF_FILES) $(HTML_FILES) $(BOOK_FILES) coderay-asciidoctor.css
	rm -f washing-machine-??.adoc steps.adoc
	rm -f snek.css snek.svg snek-theme.yml
