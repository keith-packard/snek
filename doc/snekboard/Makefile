SNEK_NO_BUILD_TARGETS = 1
SNEK_ROOT=../..

include $(SNEK_ROOT)/snek-install.defs

.SUFFIXES: .adoc .pdf .html

ADOC_FILES=snekboard.adoc

PDF_FILES=$(ADOC_FILES:.adoc=.pdf)
HTML_FILES=$(ADOC_FILES:.adoc=.html)
CSS_FILES=snek.css coderay-asciidoctor.css
IMAGE_FILES=\
	snek.svg \
	snekboard-in-enclosure.jpg \
	snekboard-label.jpg \
	snekboard.png \
	snekboard-sensors.jpg \
	snekboard-wired.jpg

THEME_FILE=$(SNEK_ROOT)/doc/snek-theme.yml

all: $(HTML_FILES) $(PDF_FILES)

LETTER_ATTRIBUTES=--attribute='pdf-style=snek' 
ATTRIBUTES=--attribute="revdate=$(SNEK_DATE)" --attribute="version=$(SNEK_VERSION)"

.adoc.html:
	asciidoctor -v $(ATTRIBUTES) -b html5 $*.adoc

.adoc.pdf:
	asciidoctor-pdf $(ATTRIBUTES) $(LETTER_ATTRIBUTES) $*.adoc

$(PDF_FILES): $(THEME_FILE) $(IMAGE_FILES)

$(HTML_FILES): snek.css

DOCFILES=$(PDF_FILES) $(HTML_FILES) $(CSS_FILES) $(IMAGE_FILES)

install: $(DOCFILES)
	install -d $(DESTDIR)$(DOCDIR)
	for i in $(DOCFILES); do install --mode=644 "$$i" $(DESTDIR)$(DOCDIR) || exit 1; done

upload: $(DOCFILES)
	rsync -avz $(DOCFILES) ../fonts $(SNEKDOC)/snekboard

snek.css: $(SNEK_ROOT)/doc/snek.css
	cp $^ $@

snek.svg: $(SNEK_ROOT)/snek.svg
	cp $^ $@

coderay-asciidoctor.css: $(HTML_FILES)

clean::
	rm -f $(PDF_FILES) $(HTML_FILES) $(BOOK_FILES)
	rm -f snek.css coderay-asciidoctor.css
	rm -f snek.svg
