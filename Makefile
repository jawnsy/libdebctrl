SUBDIRS = \
 docs     \
 src

default: all

all: build

build:
	@for i in $(SUBDIRS); do \
		echo "build ==> $$i";\
		$(MAKE) -C $$i || exit;\
	done

install: build
	for i in $(SUBDIRS); do \
		echo "install ==> $$i";\
		$(MAKE) -C $$i install || exit;\
	done

clean:
	@for i in $(SUBDIRS); do \
		echo "clean ==> $$i";\
		$(MAKE) -C $$i clean || exit;\
	done

.PHONY: default all build install clean
