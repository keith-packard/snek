SNEK_NO_BUILD_TARGETS = 1
SNEK_ROOT=..

include $(SNEK_ROOT)/snek-install.defs

.SUFFIXES: .adoc .pdf .html

ADOC_FILES=snek.adoc

PDF_FILES=$(ADOC_FILES:.adoc=.pdf)
HTML_FILES=$(ADOC_FILES:.adoc=.html)
CSS_FILES=snek.css coderay-asciidoctor.css
IMAGE_FILES=../snek.svg

all: $(HTML_FILES) $(PDF_FILES) all-lessons

BOOK_FILES=snek-book.pdf snek-cover.pdf snek-book-book.pdf snek-cover-book.pdf

book: $(BOOK_FILES)

LETTER_ATTRIBUTES=--attribute='pdf-theme=snek' 
BOOK_ATTRIBUTES=--attribute='pdf-theme=snek-book' 
ATTRIBUTES=--attribute="revdate=$(SNEK_DATE)" --attribute="version=$(SNEK_VERSION)"

.adoc.html:
	asciidoctor -v -B$(SNEK_ROOT) $(ATTRIBUTES) -b html5 $*.adoc

.adoc.pdf:
	asciidoctor-pdf -B$(SNEK_ROOT) $(ATTRIBUTES) $(LETTER_ATTRIBUTES) $*.adoc

$(PDF_FILES): snek-theme.yml $(IMAGE_FILES)

DOCFILES=$(PDF_FILES) $(HTML_FILES) $(CSS_FILES) $(IMAGE_FILES)

install: $(DOCFILES) install-lessons
	install -d $(DESTDIR)$(DOCDIR)
	for i in $(DOCFILES); do install --mode=644 "$$i" $(DESTDIR)$(DOCDIR) || exit 1; done

snek-book.pdf: snek.adoc snek-book-theme.yml
	asciidoctor-pdf -o `pwd`/$@ -B$(SNEK_ROOT) $(ATTRIBUTES) $(BOOK_ATTRIBUTES) snek.adoc

snek-book-book.pdf: snek-book.pdf
	pdfbook2 -n --paper=letter snek-book.pdf

snek-cover.pdf: snek-cover.svg
	rsvg-convert -d 72 -p 72 -f pdf -o $@ $^

snek-cover-book.pdf: snek-cover.svg
	rsvg-convert -d 69.05 -p 69.94 -f pdf -o $@ $^

upload: $(DOCFILES) upload-lessons
	rsync -avz $(DOCFILES) fonts $(SNEKDOC)

coderay-asciidoctor.css: $(HTML_FILES)

clean:: clean-lessons
	rm -f $(PDF_FILES) $(HTML_FILES) $(BOOK_FILES) coderay-asciidoctor.css

all-lessons:
	+for i in lessons/lesson-*; do make -C $$i all; done

clean-lessons:
	+for i in lessons/lesson-*; do make -C $$i clean; done

upload-lessons:
	+for i in lessons/lesson-*; do make -C $$i upload; done

install-lessons:
	+for i in lessons/lesson-*; do make -C $$i DESTDIR=$(DESTDIR) DOCDIR=$(DOCDIR) install; done
