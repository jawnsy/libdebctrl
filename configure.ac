# Process this file with autoconf to produce a configure script.

# We built this file with autoconf 2.67
AC_PREREQ([2.67])

# Initialize autoconf and automake
AC_INIT([libdebctrl], [0.4], [jawnsy@cpan.org])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Enable friendlier short "silent" rules by default
AM_SILENT_RULES([yes])

# If we don't have src/parser.c, we're in the wrong place
AC_CONFIG_SRCDIR([src/parser.c])

# Provide configuration details in config.h
AC_CONFIG_HEADERS([include/config.h])

# Checks for programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Check that Doxygen is available
AC_CHECK_PROGS([DOXYGEN], [doxygen])

# Make sure libtool is available and up-to-date
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)
AC_CONFIG_MACRO_DIR([m4])
LT_INIT

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdlib.h string.h strings.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([isascii strcasecmp strdup strerror strndup strrchr strtoul strlcpy])

#  docs/Makefile
#  examples/Makefile
#  examples/display/Makefile

AC_CONFIG_FILES([
  Makefile
  src/Makefile
])
AC_OUTPUT
