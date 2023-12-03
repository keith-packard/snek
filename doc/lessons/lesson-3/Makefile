SNEK_NO_BUILD_TARGETS = 1
SNEK_ROOT=../../..

include $(SNEK_ROOT)/snek-install.defs

.SUFFIXES: .adoc .pdf .html

ADOC_FILES=lesson-3-bumper-car.adoc

PDF_FILES=$(ADOC_FILES:.adoc=.pdf)
HTML_FILES=$(ADOC_FILES:.adoc=.html)
CSS_FILES=snek.css  coderay-asciidoctor.css
IMAGE_FILES=\
	bumper-car-annotated.svg \
	bumper-car.svg \
	21980.dat.png \
	22253c02.dat.png \
	2431dm9.dat.png \
	2654.dat.png \
	3001.dat.png \
	32073.dat.png \
	3647.dat.png \
	3648b.dat.png \
	3703.dat.png \
	3713.dat.png \
	3738.dat.png \
	4459.dat.png \
	4519.dat.png \
	6590.dat.png \
	Bumper-Car-01.png \
	Bumper-Car-02.png \
	Bumper-Car-03.png \
	Bumper-Car-04.png \
	Bumper-Car-05.png \
	Bumper-Car-06.png \
	Bumper-Car-07.png \
	Bumper-Car-08.png \
	Bumper-Car-09.png \
	Bumper-Car-10.png \
	Bumper-Car-11.png \
	Bumper-Car-12.png \
	Bumper-Car-13.png \
	Bumper-Car-14.png \
	Bumper-Car-15.png \
	Bumper-Car-16.png

INC_FILES=steps.adoc

all: $(HTML_FILES) $(PDF_FILES)

LETTER_ATTRIBUTES=--attribute='pdf-theme=snek' 
ATTRIBUTES=--attribute="revdate=$(SNEK_DATE)" --attribute="version=$(SNEK_VERSION)"

.adoc.html:
	asciidoctor -v -B$(SNEK_ROOT) -B. $(ATTRIBUTES) -b html5 $*.adoc

.adoc.pdf:
	asciidoctor-pdf -a optimize -B$(SNEK_ROOT) -B. $(ATTRIBUTES) $(LETTER_ATTRIBUTES) $*.adoc

$(PDF_FILES): snek-theme.yml $(IMAGE_FILES) $(INC_FILES) snek.svg

DOCFILES=$(PDF_FILES) $(HTML_FILES) $(CSS_FILES) snek.svg $(IMAGE_FILES)

$(HTML_FILES): $(INC_FILES)

install: $(PDF_FILES)
	install -d $(DESTDIR)$(DOCDIR)/lessons
	for i in $(PDF_FILES); do install --mode=644 "$$i" $(DESTDIR)$(DOCDIR)/lessons || exit 1; done

upload: $(DOCFILES)
	rsync -avz $(DOCFILES) ../../fonts $(SNEKDOC)/lessons/lesson-3

steps.adoc: $(SNEK_DOC_UTIL)/make-step $(SNEK_DOC_UTIL)/make-steps
	$(SNEK_DOC_UTIL)/make-steps Bumper-Car-??.html > $@

snek.css: $(SNEK_DOC)/snek.css
	cp $^ $@

snek.svg: $(SNEK_ROOT)/snek.svg
	cp $^ $@

snek-theme.yml: $(SNEK_DOC)/snek-theme.yml
	cp $^ $@

coderay-asciidoctor.css: $(HTML_FILES)

clean::
	rm -f $(PDF_FILES) $(HTML_FILES) $(BOOK_FILES) coderay-asciidoctor.css
	rm -f Bumper-Car-??.adoc steps.adoc
	rm -f snek.css snek.svg snek-theme.yml
