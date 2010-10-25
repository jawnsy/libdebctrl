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
#include <debctrl/validate.h>

static dcStatus dc_control_parse_package(
  dcControl *control,
  const char *name,
  dcParserBlock *block
);

/**
 * Information about records in Source package control files
 *
 * This value correlates names of control fields with functions used to handle
 * them. Each value of this type consists of a field name (in the preferred
 * CamelCase format, but where case is irrelevant for determining equivalence),
 * along with a hook to a processing function
 *
 * The processing function must consume the entire block, emit warning/error
 * conditions via the \ref dcErrorHandler and \ref dcStatus mechanisms, and
 * update the \ref dcControl object accordingly.
 */
struct _dcControlField
{
  const char *name;
  dcStatus (*hook)(
    dcControl *,
    const char *,
    dcParserBlock *
  );
};
typedef struct _dcControlField dcControlField;

/**
 * Table of Control Field parsing functions
 *
 * This is a simple sorted table (lookups occur using binary searches, since
 * all fields are about as likely to occur, and only occur once per section).
 *
 * It contains all field names as defined by Debian Policy, excluding special
 * fields such as the X[BS]-Comment fields.
 */
static const dcControlField dc_field_table[] = {
  { "Package",                &dc_control_parse_package },
  { "Source",                 &dc_control_parse_package }
};
#define FIELD_TABLE_SIZE   2 /**< Number of elements in \c dc_field_table */

/**
 * Comparison function for Control Field records
 *
 * This is a simple comparison function (using strcasecmp) to compare two
 * \ref dcControlField objects, passed to \c bsearch.
 */
static int dc_field_compare(
  const void *f1,
  const void *f2
) {
  const dcControlField *dcf1 = (dcControlField *) f1;
  const dcControlField *dcf2 = (dcControlField *) f2;

  return strcasecmp(dcf1->name, dcf2->name);
}

/**
 * Initialize a Control Source package
 *
 * For details on the structure and its fields, see \ref dcControlSource
 */
void dc_control_source_init(
  dcControlSource *source
) {
  assert(source != NULL);

  source->name = NULL;
}

/**
 * Clear all information from a Control Source package
 *
 * Given a \ref dcControlSource instance, this will free internally-allocated
 * memory, allowing it to be re-used if needed.
 *
 * \param[in,out] source A pointer to a Control Source instance
 */
void dc_control_source_clear(
  dcControlSource *source
) {
  assert(source != NULL);

  free(source->name);
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

  /* initialize static dcControlSource part */
  dc_control_source_init(&control->source);

  return control;
}

/**
 * Parse Control Source package data from a dcParser
 *
 * \param[in,out] control A pointer to a Control instance
 * \param[in] section A pointer to the head dcParserSection
 *
 * \retval dcNoErr if the operation completed successfully
 * \retval dcParameterErr if the input parameters are invalid
 * \retval dcMemFullErr if there was a failure to allocate memory
 * \retval dcSyntaxErr if there was invalid data in the file
 */
dcStatus dc_control_parse(
  dcControl *control,
  dcParserSection *section
) {
  dcParserBlock *block;
  dcControlField needle; /* find this in the dc_field_table haystack */
  dcControlField *res; /* result if found */

  assert(control != NULL);
  assert(section != NULL);

  if (control == NULL || section == NULL)
    return dcParameterErr;

  block = section->head;
  while (block != NULL)
  {
    needle.name = block->name;
    res = bsearch(&needle, dc_field_table, FIELD_TABLE_SIZE,
      sizeof(dcControlField), dc_field_compare);

    if (res == NULL)
      dc_warn(&control->handler, &block->head->ctx, _("Ignoring unknown "
        "source package control field '%s'"), needle.name);
    else
      (*res->hook)(control, res->name, block);

    block = block->next;
  }

  return dcNoErr;
}

/**
 * Parse a package name (helper function)
 *
 * This is an internal helper function used to parse package names (from the
 * Source or Package fields), validating them using \ref dc_valid_package.
 *
 * \param[in,out] control A pointer to a Control instance
 * \param[in] name The 'proper' name of the Debian control field
 * \param[in] block A pointer to the current dcParserBlock
 *
 * \retval dcNoErr if the operation completed successfully
 * \retval dcParameterErr if the input parameters are invalid
 * \retval dcMemFullErr if there was a failure to allocate memory
 */
static dcStatus dc_control_parse_package(
  dcControl *control,
  const char *name,
  dcParserBlock *block
) {
  dcParserChunk *chunk;

  assert(control != NULL);
  assert(block != NULL);

  if (control == NULL || block == NULL)
    return dcParameterErr;

  chunk = block->head;

  switch (dc_valid_package(chunk->text))
  {
    case dcPackageLengthErr:
      dc_warn(&control->handler, &chunk->ctx, _("Package names must be at "
        "least two characters long (Sec. 5.6.1)"));
      break;
    case dcPackagePrefixErr:
     dc_warn(&control->handler, &chunk->ctx, _("Package names must begin "
       "with a number or lower-case letter (Sec. 5.6.1)"));
      break;
    case dcPackageInvalidErr:
     dc_warn(&control->handler, &chunk->ctx, _("Package names must contain "
       "only lower-case alphabetic, numeric, or '+', '-', and '.' "
       "characters (Sec. 5.6.1)"));
      break;
    case dcNoErr:
      break;
    default:
      /* unreachable */
      break;
  }

  /* A package name is either a Source or Package line */
  if (*name == 'S')
  {
    if (chunk->text == NULL)
      control->source.name = NULL;
    else
    {
      control->source.name = strdup(chunk->text);
      if (control->source.name == NULL)
        return dcMemFullErr;
    }
  }
  else /* then (*name == 'P') */
  {
    // XXX: attach this to tail section
//    control->
  }

  chunk = chunk->next;
  if (chunk != NULL)
  {
    dc_warn(&control->handler, &chunk->ctx, _("Ignoring unexpected "
      "continuation data in '%s' field"), name);
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

  /* free allocated memory from dcControlSource */
  dc_control_source_clear(&(*ptr)->source);

  free(*ptr);
  *ptr = NULL;
}
