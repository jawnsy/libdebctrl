/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * Generic utilities for libdebctrl
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * This provides utilities for:
 * \li string manipulation
 * which are used internally by libdebctrl.
 */

#ifndef DEBCTRL_UTIL_H
#define DEBCTRL_UTIL_H

#include <debctrl/common.h>

char * dc_strchomp(char *text, size_t len);
const char * dc_strchug(const char *text);

/**
 * An automatically-expanding string buffer
 *
 * String buffers provide a convenient way to build relatively large (> 4 kB)
 * chunks of text (e.g. when flattening a complex data structure). It expands
 * the buffer as necessary using \c realloc.
 *
 * \note The buffer is expanded by \ref STRING_STEP_SIZE bytes as required.
 * Extraneous memory is not freed until the string is destroyed, or trimmed
 * using \ref dc_string_resize.
 */
struct _dcString
{
  char *text;    /**< Pointer to start of string buffer */

  size_t len;    /**< Length of string (excluding \c NUL byte) */
  size_t size;   /**< Allocated buffer size (must be at least len+1) */
};
/* related methods */
dcString * dc_string_new(size_t size);
void dc_string_append(dcString *string, const char *text);
void dc_string_append_c(dcString *string, char c);
int dc_string_resize(dcString *string, size_t size);
void dc_string_free(dcString **ptr);

#endif /* DEBCTRL_UTIL_H */

