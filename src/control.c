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
#include <strings.h>  /* for: strcasecmp */

#include <debctrl/parser.h>
#include <debctrl/error.h>
#include <debctrl/control.h>

/**
 * Construct a Control Source package
 *
 * For details on the structure and its fields, see \ref dcControlSource
 *
 * \retval NULL if there is a failure to allocate memory
 * \return a dynamically allocated dcControlSource object
 */
dcControlSource * dc_control_source_new(
  void
) {
  dcControlSource *source = NEW(dcControlSource);

  if (source == NULL)
    return NULL;

  return source;
}

/**
 * Destroy a Control Source package
 *
 * Given a \ref dcControlSource instance that was allocated by
 * \ref dc_control_source_new, this will free internally-allocated memory
 * before destroying the control source package itself.
 *
 * \param[in,out] ptr The address of a pointer to a Control Source instance
 *
 * \note The pointer will be set to \c NULL after memory is freed.
 */
void dc_control_source_free(
  dcControlSource **ptr
) {
  assert(ptr != NULL);
  assert(*ptr != NULL);

  if ((*ptr)->name != NULL)
    free((*ptr)->name);

  free(*ptr);
  *ptr = NULL;
}

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
  dc_error_handler_init(&control->handler);

  return control;
}

/**
 * Parse Control Source package data from a dcParser
 *
 * \param[in,out] control A pointer to a Control instance
 * \param[in] head A pointer to the head dcParserSection
 *
 * \retval dcMemFullErr if there was a failure to allocate memory
 * \retval dcParameterErr if the parameters are invalid
 * \retval dcSyntaxErr if there was invalid data in the file
 * \retval dcNoErr if the operation completed successfully
 */
dcStatus dc_control_parse(
  dcControl *control,
  dcParserSection *head
) {
  dcParserBlock *block;

  assert(control != NULL);
  assert(head != NULL);

  if (control == NULL || head == NULL)
    return dcParameterErr;

  block = head->head;
  if (block != NULL)
  {
    if (strcasecmp(block->name, "Source") == 0)
    {
      control->source.name = strdup(block->head->text);
    }
  }

  return dcNoErr;
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
