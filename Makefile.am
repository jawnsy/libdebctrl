AUTOMAKE_OPTIONS = foreign

AM_CFLAGS = $(WARNFLAGS)
ACLOCAL_AMFLAGS = -I m4

include_debctrldir = $(includedir)/debctrl

include_HEADERS =               \
 include/debctrl.h

include_debctrl_HEADERS =       \
 include/debctrl/common.h       \
 include/debctrl/control.h      \
 include/debctrl/defaults.h     \
 include/debctrl/error.h        \
 include/debctrl/parser.h       \
 include/debctrl/util.h         \
 include/debctrl/validate.h     \
 include/debctrl/version.h

# docs
SUBDIRS = \
 src      \
 examples

DISTCLEANFILES = *~ include/*~
