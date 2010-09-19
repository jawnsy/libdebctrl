EXAMPLES = \
 display

default: all

all: build

build:
	@for i in $(EXAMPLES); do \
		echo "build ==> $$i";\
		$(MAKE) -C $$i || exit;\
	done

clean:
	@for i in $(EXAMPLES); do \
		echo "clean ==> $$i";\
		$(MAKE) -C $$i clean || exit;\
	done

.PHONY: default all build clean
