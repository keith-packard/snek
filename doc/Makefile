SNEK_NO_BUILD_TARGETS = 1
SNEK_ROOT=..

include $(SNEK_ROOT)/snek-install.defs

.SUFFIXES: .adoc .pdf .html

ADOC_FILES=snek.adoc

PDF_FILES=$(ADOC_FILES:.adoc=.pdf)
HTML_FILES=$(ADOC_FILES:.adoc=.html)
CSS_FILES=snek.css coderay-asciidoctor.css
IMAGE_FILES=../snek.svg

all: $(HTML_FILES) $(PDF_FILES)

ATTRIBUTES= --attribute="revdate=$(SNEK_DATE)" --attribute="version=$(SNEK_VERSION)"

.adoc.html:
	asciidoctor -B$(SNEK_ROOT) $(ATTRIBUTES) -b html5 $*.adoc

.adoc.pdf:
	asciidoctor-pdf -B$(SNEK_ROOT) $(ATTRIBUTES) $*.adoc

$(PDF_FILES): snek-theme.yml $(IMAGE_FILES)

DOCFILES=$(PDF_FILES) $(HTML_FILES) $(CSS_FILES)

install: $(DOCFILES)
	install -d $(DESTDIR)$(DOCDIR)
	for i in $(DOCFILES); do install --mode=644 "$$i" $(DESTDIR)$(DOCDIR); done

upload: $(DOCFILES)
	rsync -avz $(DOCFILES) fonts $(SNEKDOC)

clean::
	rm -f $(PDF_FILES) $(HTML_FILES)
