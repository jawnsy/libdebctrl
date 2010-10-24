/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * Version handling facilities for libdebctrl
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * This package provides a representation of a package version, as well as
 * some utilities to manipulate and compare them.
 *
 * \see "5.6.12 Version", from the Debian Policy Manual:
 * http://www.debian.org/doc/debian-policy/ch-controlfields.html#s-f-Version
 */

#include <string.h>
#include <stdio.h>

#include <debctrl/version.h>

/**
 * Construct a dcVersion from a string
 *
 * A dcVersion is a representation of a Debian package version, as defined by
 * Debian Policy 5.6.12. Package version numbers can optionally have an epoch
 * number and Debian revision, and are specified in the format:
 * <em>[epoch:]upstream_version[-debian_revision]</em>
 *
 * \retval NULL if there is a failure to allocate memory (either for dcVersion
 * or its internal strings)
 * \return a dynamically allocated dcVersion object
 * * \see The associated data validation routine, \ref dc_valid_version
 */
dcVersion * dc_version_new(
  void
) {
  dcVersion *version;

  version = NEW(dcVersion);
  if (version == NULL)
    return NULL;

  version->epoch = 0;
  version->version = NULL;
  version->revision = NULL;

  return version;
}

/**
 * Parse a string into a dcVersion representation
 *
 * This function parses a version string and separates it into its
 * constituent parts.
 *
 * \param[in,out] version A pointer to a dcVersion object
 * \param[in] vstring A string containing a version number
 *
 * \retval dcNoErr if the operation completed successfully
 * \retval dcMemFullErr if there was a failure to allocate memory
 * \retval dcParameterErr if the input parameters are invalid
 *
 * \note This function is safe with a previously-initialized version
 * object. It will free dynamically allocated memory prior to parsing.
 *
 * \see The associated data validation routine, \ref dc_valid_version
 */
dcStatus dc_version_set(
  dcVersion *version,
  const char *vstring
) {
  char *buf, *ptr;
  char *uversion;
  char *dversion = NULL;

  assert(version != NULL);
  assert(vstring != NULL);

  if (version == NULL || vstring == NULL)
    return dcParameterErr;

  /* reset all internal parameters */
  version->epoch = 0;
  if (version->version != NULL)
  {
    free(version->version);
    version->version = NULL;
  }
  if (version->revision != NULL)
  {
    free(version->revision);
    version->revision = NULL;
  }

  /* duplicate buffer for our use */
  buf = strdup(vstring);
  if (buf == NULL)
  {
    free(version);
    return dcMemFullErr;
  }

  /* start scanning from the beginning of buf */
  ptr = buf;
  while (*ptr != '\0')
  {
    /* we have an epoch, mark the segment until the : */
    if (*ptr == ':')
      break;
    ptr++;
  }

  /* if epoch was found, handle it */
  if (*ptr == ':')
  {
    *ptr = '\0'; /* change : to \0 to mark end of epoch */
    uversion = ptr+1; /* version begins next */
    ptr = buf; /* reset to start of epoch */

    /* ensure epoch contains only numeric characters */
    while (*ptr != '\0')
    {
      if (!(*ptr >= '0' && *ptr <= '9'))
      {
        free(buf);
        return dcParameterErr;
      }
      ptr++;
    }

    /* if we reach here, the string is valid and *ptr is at the epoch '\0'
     * convert the epoch using base 10
     */
    version->epoch = strtoul(buf, NULL, 10);
  }
  else /* ptr is at '\0', version begins at start of buf */
    uversion = buf;

  /* scan the string for revision number */
  ptr = uversion;
  while (*ptr != '\0')
  {
    /* keep track of last '-' seen (if any) */
    if (*ptr == '-')
    {
      dversion = ptr+1;
    }
    ptr++;
  }

  /* if a revision was found, the character immediately preceding it is
   * a '-' and must be set to NUL (to terminate version part)
   */
  if (dversion != NULL)
  {
    *(dversion-1) = '\0';
    version->revision = strdup(dversion);
  }

  if (uversion != NULL)
    version->version = strdup(uversion);

  free(buf);
  return dcNoErr;
}

/**
 * Destroy a dcVersion
 *
 * Given a dcVersion that was allocated by \ref dc_version_new, this will free
 * internally-allocated memory before destroying the version object itself.
 *
 * \param[in] ptr The address of a pointer to a dcVersion
 *
 * \note The pointer will be set to \c NULL after memory is freed.
 */
void dc_version_free(
  dcVersion **ptr
) {
  assert(ptr != NULL);
  assert(*ptr != NULL);

  if ((*ptr)->version != NULL)
    free((*ptr)->version);

  if ((*ptr)->revision != NULL)
    free((*ptr)->revision);

  free(*ptr);
  *ptr = NULL;
}
