PREFIX ?= /usr/local

CFLAGS = -std=c99 -pedantic -Wall -g -O0 -DDEBUG

MANDATORY_CFLAGS := -I.

MANDATORY_LDFLAGS := -L$(PREFIX)/lib

ALL_CFLAGS += $(CFLAGS) $(MANDATORY_CFLAGS) -fPIC
ALL_LDFLAGS += $(LDFLAGS) $(MANDATORY_LDFLAGS) -fPIC

LIBS = -ldebctrl

ifeq ($(DEBUG), 1)
	NICE_CC = $(CC)
	NICE_AR = $(AR)
else
	NICE_CC = @echo "  CC  $@"; $(CC)
	NICE_AR = @echo "  AR  $@"; $(AR)
endif

# objects to build
OBJS =          \
 debug.o        \
 display.o

default: all

all: display

display: $(OBJS)
	$(NICE_CC) $(ALL_LDFLAGS) $(OBJS) $(LIBS) -o display

.c.o:
	$(NICE_CC) $(ALL_CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) display

.PHONY: default all clean
