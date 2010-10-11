/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * Common support declarations
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * This provides common supporting declarations for libdebctrl.
 * - It includes system headers:
 *   - \c stdlib.h (for malloc, realloc, free)
 *   - \c assert.h (for assert)
 *   - \c stddef.h (for NULL, size_t)
 * - It defines macros used throughout libdebctrl:
 *   - \ref NEW
 *   - \ref _
 * - It provides forward declarations for libdebctrl types.
 *
 * \par Assertion system
 * libdebctrl's assertion system uses your C library's \c assert function to
 * provide some basic sanity checks to ensure function input seems reasonable.
 * All assertions are optimized away if \c NDEBUG is defined.
 *
 * \par Internationalisation (I18n) support
 * This library is prepared for future internationalisation using GNU gettext.
 * All error messages are passed through the \c _() macro.
 */

#ifndef DEBCTRL_COMMON_H
#define DEBCTRL_COMMON_H

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include <debctrl/defaults.h>

/* prepare for future internationalisation */
#define _(x) (x)

/**
 * This macro dynamically creates an object of type \c t using \c malloc.
 *
 * \param[in] t The type of object for which to allocate memory
 *
 * \retval NULL if there is an allocation failure
 * \returns A block of memory large enough for type \c t
 *
 * \hideinitializer
 */
#define NEW(t)              malloc(sizeof (t))

/* Forward declarations for libdebctrl types */
typedef struct _dcParser           dcParser;
typedef struct _dcParserBlock      dcParserBlock;
typedef struct _dcParserChunk      dcParserChunk;
typedef struct _dcParserContext    dcParserContext;
typedef struct _dcParserSection    dcParserSection;

typedef struct _dcControl          dcControl;
typedef struct _dcControlSource    dcControlSource;

typedef struct _dcErrorHandler     dcErrorHandler;

typedef struct _dcString           dcString;

typedef struct _dcVersion          dcVersion;

#endif /* DEBCTRL_COMMON_H */
