/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * Debian source package control file parser
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * Processing Debian's package metadata ("control") files occurs in two steps:
 * -# Text is parsed into a data structure representation (syntax)
 * -# Specific data is extracted from the data structure (semantics)
 *
 * This parser is concerned with extracting \em semantic information about a
 * \c debian/control (source package) file, given \ref dcParserSection objects
 * (these usually come directly from the \ref dcParser instance).
 *
 * Because this parser understands control file semantics, it can provide some
 * rudimentary data validation features as well as providing an interface for
 * manipulating the metadata programmatically.
 *
 * \bug All error messages are in English and are not internationalized
 *
 * \see "Control files and their fields", from the Debian Policy Manual:
 * http://www.debian.org/doc/debian-policy/ch-controlfields.html
 */

#include <string.h>   /* for: strdup */

#include <debctrl/parser.h>
#include <debctrl/error.h>
#include <debctrl/control.h>

/**
 * Construct a Control parser instance
 *
 * For details on the structure and its fields, see \ref dcControl
 *
 * \retval NULL if there is a failure to allocate memory
 * \return a dynamically allocated dcControl object
 */
dcControl * dc_control_new(
  void
) {
  dcControl *control = NEW(dcControl);

  if (control == NULL)
    return NULL;

  /* set up default error handler */
  control->handler = dc_error_handler_new();

  return control;
}

/**
 * Destroy a Control parser instance
 *
 * Given a \ref dcControl instance that was allocated by \ref dc_control_new,
 * this will automatically free internally-allocated memory before destroying
 * the parser itself.
 *
 * \param[in,out] ptr The address of a pointer to a Control instance
 *
 * \note The pointer will be set to \c NULL after memory is freed.
 */
void dc_control_free(
  dcControl **ptr
) {
  assert(ptr != NULL);
  assert(*ptr != NULL);

  free(*ptr);
  *ptr = NULL;
}
