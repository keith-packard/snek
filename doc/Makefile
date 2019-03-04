SNEK_NO_BUILD_TARGETS = 1
SNEK_ROOT=..

include $(SNEK_ROOT)/snek-install.defs

.SUFFIXES: .adoc .pdf .html

ADOC_FILES=snek.adoc

PDF_FILES=$(ADOC_FILES:.adoc=.pdf)
HTML_FILES=$(ADOC_FILES:.adoc=.html)
CSS_FILES=snek.css

all: $(HTML_FILES) $(PDF_FILES)

.adoc.html:
	asciidoctor -b html5 $*.adoc

.adoc.pdf:
	asciidoctor-pdf $*.adoc

$(PDF_FILES): snek-theme.yml

DOCFILES=$(PDF_FILES) $(HTML_FILES) $(CSS_FILES)

install: $(DOCFILES)
	install -d $(DESTDIR)$(DOCDIR)
	for i in $(DOCFILES); do install --mode=644 "$$i" $(DESTDIR)$(DOCDIR); done

upload:
	scp -rp $(DOCFILES) fonts $(SNEKDOC)

clean::
	rm -f $(PDF_FILES) $(HTML_FILES)
