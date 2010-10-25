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

#include <string.h> /* for strdup, strrchr */

#include <debctrl/version.h>
#include <debctrl/util.h>

/**
 * Construct a dcVersion
 *
 * A dcVersion is a representation of a Debian package version, as defined by
 * Debian Policy 5.6.12.
 *
 * \retval NULL if there is a failure to allocate memory
 * \return a dynamically allocated dcVersion object
 *
 * \see The associated mutator method, \ref dc_version_set and data validation
 * routine, \ref dc_valid_version
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
 * Extract version information from a string
 *
 * This method extracts version information from a string and performs some
 * rudimentary validation checking.
 *
 * Package version numbers can optionally have an epoch number and Debian
 * revision, and are specified in the format:
 * <em>[epoch:]upstream_version[-debian_revision]</em>
 *
 * \param[in,out] version A pointer to a dcVersion object
 * \param[in] vstring A package version in string format
 *
 * \retval dcNoErr if the operation completed successfully
 * \retval dcParameterErr if the input parameters are invalid
 */
dcStatus dc_version_set(
  dcVersion *version,
  const char *vstring
) {
  const char *ptr;
  const char *hyphen;

  assert(version != NULL);
  assert(vstring != NULL);

  if (version == NULL || vstring == NULL)
    return dcParameterErr;

  /* reset all internal parameters */
  dc_version_clear(version);

  /* attempt to read epoch into unsigned long using base 10. If *ptr
   * is not ':', then we had an error
   */
  version->epoch = strtoul(vstring, (char **) &ptr, 10);

  if (*ptr == ':')
  {
    /* ensure epoch contains only numeric characters */
    ptr = vstring;
    while (*ptr != ':')
    {
      if (!(*ptr >= '0' && *ptr <= '9'))
      {
        return dcParameterErr;
      }
      ptr++;
    }
  }
  else
  {
    /* there is no epoch, default to zero and reset pointer to address prior
     * to string so ptr+1 is at the beginning
     */
    version->epoch = 0;
    ptr = vstring-1;
  }

  hyphen = strrchr(vstring, '-');

  /* if we can't find a hyphen, ptr+1 to end is version */
  if (hyphen == NULL)
  {
    version->version = strdup(ptr+1);
    if (version->version == NULL)
      goto memerr;
  }
  else
  {
    version->version = dc_strndup(ptr+1, hyphen - (ptr+1));
    if (version->version == NULL)
      goto memerr;

    version->revision = strdup(hyphen+1);
    if (version->revision == NULL)
      goto memerr;
  }

  return dcNoErr;

memerr:
  dc_version_clear(version);
  return dcMemFullErr;
}

/**
 * Clear internal memory for a dcVersion
 *
 * This function resets all internally-stored information stored within a
 * given dcVersion object.
 *
 * \param[in,out] version A pointer to a dcVersion object
 *
 * \note All pointers will be set to \c NULL after memory is freed.
 */
void dc_version_clear(
  dcVersion *version
) {
  assert(version != NULL);

  version->epoch = 0;

  free(version->version);
  version->version = NULL;

  free(version->revision);
  version->revision = NULL;
}

/**
 * Destroy a dcVersion
 *
 * Given a dcVersion that was allocated by \ref dc_version_new, this will free
 * internally-allocated memory before destroying the version object itself.
 *
 * \param[in,out] ptr The address of a pointer to a dcVersion
 *
 * \note The pointer will be set to \c NULL after memory is freed.
 */
void dc_version_free(
  dcVersion **ptr
) {
  assert(ptr != NULL);
  assert(*ptr != NULL);

  dc_version_clear(*ptr);

  free(*ptr);
  *ptr = NULL;
}
