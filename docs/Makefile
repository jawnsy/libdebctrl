ifeq ($(V), 1)
	NICE_DOXYGEN = doxygen
else
	NICE_DOXYGEN = @echo "  DOXYGEN  $@"; doxygen
endif

default: all

all: docs

docs:
	$(NICE_DOXYGEN) Doxyfile

author:
	$(NICE_DOXYGEN) Doxyfile.author
	@echo "Generating PDF documentation..."
	make -C latex refman.pdf

PREFIX ?= /usr/local
INSTALLPATH = $(DESTDIR)$(PREFIX)
DOC_DIR=$(INSTALLPATH)/share/doc/libdebctrl/

install:
	mkdir -p $(DOC_DIR)
	rm -fr $(DOC_DIR)
	cp -a html/ $(DOC_DIR)

clean:
	rm -rf html latex

.PHONY: all clean default docs
