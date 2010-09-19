PREFIX ?= /usr/local

# DESTDIR is a Debian extension; it allows the root to be moved for building
# to a different destination, intended to be accessed as $(PREFIX) --
# i.e., for a chroot
INSTALLPATH = $(DESTDIR)$(PREFIX)

VERSION = 0.4
ABI_VERSION = 0

# We are using C99 + POSIX + XSI
CFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700

# Enable lots of warnings
CFLAGS += -pedantic -Wall -W

# These flags are useful to catch problems during development, but may
# cause problems with different versions of gcc (older than 4.3)
#
# -Wformat=2 -Wmissing-prototypes -Wstrict-prototypes -Wnested-externs \
# -Wmissing-declarations -Wpointer-arith \
# -Wendif-labels -Wbad-function-cast -Wwrite-strings -Wold-style-definition

# Tell the compiler where to find our includes
CFLAGS += -I../include

ifeq ($(DEBUG), 1)
	CFLAGS += -Werror -g -O0
else
	CFLAGS += -O3 -DNDEBUG
endif

MANDATORY_CFLAGS :=

MANDATORY_LDFLAGS :=

ALL_CFLAGS += $(CFLAGS) $(MANDATORY_CFLAGS) -fPIC
ALL_LDFLAGS += $(LDFLAGS) $(MANDATORY_LDFLAGS) -fPIC

ifeq ($(V), 1)
	NICE_CC = $(CC)
	NICE_AR = $(AR)
else
	NICE_CC = @echo "  CC  $@"; $(CC)
	NICE_AR = @echo "  AR  $@"; $(AR)
endif

# objects to build
OBJS =          \
 control.o      \
 error.o        \
 parser.o       \
 util.o

default: all

all: library

library: $(OBJS) libdebctrl.so libdebctrl.a libdebctrl.pc

libdebctrl.a: $(OBJS)
	$(NICE_AR) cr libdebctrl.a $(OBJS)

libdebctrl.so: libdebctrl.so.$(ABI_VERSION)
	ln -sf libdebctrl.so.$(ABI_VERSION) libdebctrl.so

libdebctrl.so.$(ABI_VERSION): $(OBJS)
	$(NICE_CC) -Wl,-soname=$@ -shared $(ALL_LDFLAGS) $(OBJS) -o $@

libdebctrl.pc: libdebctrl.pc.in
	@echo "  GEN libdebctrl.pc"
	@cat libdebctrl.pc.in | \
		sed 's@++PREFIX++@$(PREFIX)@g' | \
		sed 's@++VERSION++@$(VERSION)@g' | \
		sed 's@++CFLAGS++@$(MANDATORY_CFLAGS)@g' | \
		sed 's@++LDFLAGS++@$(MANDATORY_LDFLAGS)@g' \
		> libdebctrl.pc

install: library libdebctrl.pc
	install -d $(INSTALLPATH)/lib
	ln -sf libdebctrl.so.$(ABI_VERSION) $(INSTALLPATH)/lib/libdebctrl.so
	install -m 0644 libdebctrl.so.$(ABI_VERSION) $(INSTALLPATH)/lib
	install -m 0644 libdebctrl.a $(INSTALLPATH)/lib
	install -d $(INSTALLPATH)/lib/pkgconfig
	install -m 0644 libdebctrl.pc $(INSTALLPATH)/lib/pkgconfig
	install -d $(INSTALLPATH)/include
	install -m 0644 ../include/debctrl.h $(INSTALLPATH)/include
	install -d $(INSTALLPATH)/include/debctrl
	install -m 0644 ../include/debctrl/common.h $(INSTALLPATH)/include/debctrl
	install -m 0644 ../include/debctrl/defaults.h $(INSTALLPATH)/include/debctrl
	install -m 0644 ../include/debctrl/error.h $(INSTALLPATH)/include/debctrl
	install -m 0644 ../include/debctrl/parser.h $(INSTALLPATH)/include/debctrl
	install -m 0644 ../include/debctrl/util.h $(INSTALLPATH)/include/debctrl
	@echo
	@echo "Please run ldconfig to update your library cache"
	@echo

%.o: %.c
	$(NICE_CC) $(ALL_CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) libdebctrl.so libdebctrl.so.$(ABI_VERSION) libdebctrl.a libdebctrl.pc
	$(RM) *.bb *.bbg *.da *.gcov *.gcno *.gcda gmon.out

.PHONY: default library install all clean
