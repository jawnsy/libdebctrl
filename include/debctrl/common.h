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
 *
 * \par Type Definitions
 * These are forward declarations for libdebctrl types defined in other header
 * files. Please see the originating structure definitions for details.
 */

#ifndef DEBCTRL_COMMON_H
#define DEBCTRL_COMMON_H

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include <debctrl/defaults.h>

/**
 * Lookup localized format strings
 *
 * This macro uses GNU Gettext to return an appropriately localized format
 * string, used most frequently for returning error messages.
 *
 * \param[in] x Default format string
 *
 * \returns An appropriately localized format string
 *
 * \bug This is currently a no-op. It is defined to prepare for future
 * localization of error strings.
 *
 * \hideinitializer
 */
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

/** \see The originating struct definition, \ref _dcParser */
typedef struct _dcParser           dcParser;
/** \see The originating struct definition, \ref _dcParserBlock */
typedef struct _dcParserBlock      dcParserBlock;
/** \see The originating struct definition, \ref _dcParserChunk */
typedef struct _dcParserChunk      dcParserChunk;
/** \see The originating struct definition, \ref _dcParserContext */
typedef struct _dcParserContext    dcParserContext;
/** \see The originating struct definition, \ref _dcParserSection */
typedef struct _dcParserSection    dcParserSection;

/** \see The originating struct definition, \ref _dcControl */
typedef struct _dcControl          dcControl;
/** \see The originating struct definition, \ref _dcControlSource */
typedef struct _dcControlSource    dcControlSource;

/** \see The originating struct definition, \ref _dcErrorHandler */
typedef struct _dcErrorHandler     dcErrorHandler;

/** \see The originating struct definition, \ref _dcString */
typedef struct _dcString           dcString;

/** \see The originating struct definition, \ref _dcVersion */
typedef struct _dcVersion          dcVersion;

/**
 * Status indication
 *
 * This enumeration provides a generic status indication for various
 * operations that can fail. Not all functions can return all status
 * values; see documentation for each function for details.
 */
typedef enum
{
  dcNoErr, /**< Operation completed successfully */
  dcParameterErr, /**< One or more parameters are invalid */
  dcMemFullErr, /**< Cannot allocate required memory */
  dcFileErr, /**< File read/write operation failure */
  dcSyntaxErr, /**< Unrecoverable syntax error in metadata */

  dcPackagePrefixErr, /**< Package name has invalid prefixing characters */
  dcPackageLengthErr, /**< Package name too short */
  dcPackageInvalidErr /**< Package name contains invalid characters */
} dcStatus;

#endif /* DEBCTRL_COMMON_H */
