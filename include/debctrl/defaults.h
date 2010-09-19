/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * Default settings
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * These are compile-time default settings for libdebctrl. They are used by
 * several different parts of code for some sane defaults.
 */

#ifndef DEBCTRL_DEFAULTS_H
#define DEBCTRL_DEFAULTS_H

/**
 * Default locale if none is specified
 *
 * If the LOCALE_ENV environment variable is empty or undefined, then we
 * default to using this one. In practice, it makes more sense to change the
 * environment variable rather than recompile everything.
 *
 * \note Per Debian Policy, control files are UTF-8 encoded, so it's best if
 * your locale also supports UTF-8 encoded strings.
 */
#define LOCALE_DEFAULT "en_CA.UTF-8"

/*
 * Extra debugging
 *
 * Using this field will enable extra debugging information to be printed,
 * which is useful during testing but may result in spurious output for
 * production use of this library
 *
 * Whether or not this is a debug build is handled by the Makefile now.
 * #define DEBUG
 */

/**
 * Default line wrapping length
 *
 * This is the length at which lines are wrapped when outputting multi-line
 * control field values. This value should also account for the space prefix
 * and newline at the end; for 80 characters, a value like 75 characters is
 * probably appropriate.
 */
#define WRAPLEN        75

/**
 * Initial size of a String
 *
 * Unless otherwise specified, strings will be \c STRING_INIT_SIZE bytes long
 * by default. See \ref dcString for details.
 *
 * \note This value should be somewhere around the average size of the control
 * files we work with, rounded up to the next page (usually 4KB).
 *
 * \bug The "average size" used here (4KB) was just selected arbitrarily. More
 * rigorous statistics would be helpful.
 */
#define STRING_INIT_SIZE      4096

/**
 * String allocation step size
 *
 * When a string grows beyond its allocated size, its allocated memory block
 * will be expanded by \c STRING_STEP_SIZE bytes each time.
 *
 * \note This value should be based on a statistical measure such as the
 * standard deviation from the mean (see \ref STRING_INIT_SIZE).
 *
 * \bug The "standard deviation" figure used here (1KB) was just selected
 * arbitrarily. More rigorous statistics would be helpful.
 */
#define STRING_STEP_SIZE      1024

#endif /* DEBCTRL_DEFAULTS_H */

