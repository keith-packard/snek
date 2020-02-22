SNEK_NO_BUILD_TARGETS = 1
SNEK_ROOT=../../..

include $(SNEK_ROOT)/snek-install.defs

.SUFFIXES: .adoc .pdf .html

ADOC_FILES=line-bug.adoc

LESSON_1=$(SNEK_ROOT)/doc/lessons/lesson-1

PDF_FILES=$(ADOC_FILES:.adoc=.pdf)
HTML_FILES=$(ADOC_FILES:.adoc=.html)
CSS_FILES=snek.css  coderay-asciidoctor.css
IMAGE_FILES=\
	line-bug.svg \
	21980.dat.png \
	22253c02.dat.png \
	2654.dat.png \
	3001.dat.png \
	32073.dat.png \
	3647.dat.png \
	3648b.dat.png \
	3713.dat.png \
	3738.dat.png \
	3895.dat.png \
	3956.dat.png \
	4459.dat.png \
	4519.dat.png \
	6590.dat.png \
	Line-Bug-01.png \
	Line-Bug-02.png \
	Line-Bug-03.png \
	Line-Bug-04.png \
	Line-Bug-05.png \
	Line-Bug-06.png \
	Line-Bug-07.png \
	Line-Bug-08.png \
	Line-Bug-09.png \
	Line-Bug-10.png \
	Line-Bug-11.png \
	Line-Bug-12.png \
	Line-Bug-13.png \
	Line-Bug-14.png \
	Line-Bug-15.png \
	Line-Bug-16.png \
	left-turn.svg \
	right-turn.svg

INC_FILES=steps.adoc

THEME_FILE=snek-theme.yml

all: $(HTML_FILES) $(PDF_FILES) $(CSS_FILES) snek.svg snek.css

LETTER_ATTRIBUTES=--attribute='pdf-style=snek' 
ATTRIBUTES=--attribute="revdate=$(SNEK_DATE)" --attribute="version=$(SNEK_VERSION)"

.adoc.html:
	asciidoctor -v -B$(SNEK_ROOT) -B. $(ATTRIBUTES) -b html5 $*.adoc

.adoc.pdf:
	asciidoctor-pdf -B$(SNEK_ROOT) -B. $(ATTRIBUTES) $(LETTER_ATTRIBUTES) $*.adoc

$(PDF_FILES): $(THEME_FILE) $(IMAGE_FILES) $(INC_FILES) ../../../snek.svg

DOCFILES=$(PDF_FILES) $(HTML_FILES) $(CSS_FILES) snek.svg $(IMAGE_FILES)

$(HTML_FILES): $(INC_FILES)

install: $(DOCFILES)
	install -d $(DESTDIR)$(DOCDIR)
	for i in $(DOCFILES); do install --mode=644 "$$i" $(DESTDIR)$(DOCDIR) || exit 1; done

upload: $(DOCFILES)
	rsync -avz $(DOCFILES) $(SNEK_ROOT)/doc/fonts $(SNEKDOC)/lessons/lesson-2/

steps.adoc: make-step make-steps
	./make-steps Line-Bug-??.html > $@

snek.css: $(LESSON_1)/snek.css
	cp $^ $@

snek.svg: $(LESSON_1)/snek.svg
	cp $^ $@

snek-theme.yml: $(SNEK_ROOT)/doc/snek-theme.yml
	cp $^ $@

coderay-asciidoctor.css: $(HTML_FILES)

clean::
	rm -f $(PDF_FILES) $(HTML_FILES) $(BOOK_FILES) coderay-asciidoctor.css
	rm -f Line-Bug-??.adoc steps.adoc
	rm -f snek.css snek.svg snek-theme.yml
