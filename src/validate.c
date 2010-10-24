/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * Debian package information validation
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * This component provides validation for Debian package information, such as
 * package and dependency names, which are common to various types of control
 * files.
 *
 * \bug All error messages are in English and are not internationalized
 *
 * \see "Control files and their fields", from the Debian Policy Manual:
 * http://www.debian.org/doc/debian-policy/ch-controlfields.html
 */

#include <ctype.h>    /* for: various isalpha/etc functions */

#include <debctrl/validate.h>

/**
 * Validate a package name
 *
 * Given a string containing the name of a Debian source or binary package,
 * this will determine whether the package name meets the specification set
 * in Debian Policy 5.6.1.
 *
 * Debian Policy stipulates that:
 * - Package names must be at least two characters long
 * - Package names must begin with a lowercase alphabetic or a numeric
 *   character
 * - Package names must contain only lowercase alphabetic, numeric, or
 *   '+', '-' and '.' characters
 *
 * \param[in] name The name of a package to validate
 *
 * \retval dcNoErr if the name appears valid
 * \retval dcParameterErr if the \c name is \c NULL
 * \retval dcPackagePrefixErr if the name begins with invalid characters
 * \retval dcPackageLengthErr if the name is too short
 * \retval dcPackageInvalidErr if the name contains invalid characters
 *
 * \see "5.6.1 Source" (and "5.6.7 Package"), from the Debian Policy Manual:
 * http://www.debian.org/doc/debian-policy/ch-controlfields.html#s-f-Source
 */
dcStatus dc_valid_package(
  const char *name
) {
  assert(name != NULL);

  if (name == NULL)
    return dcParameterErr;

  /* Package names must be at least two characters long */
  if (*name == '\0' || *(name+1) == '\0')
  {
    return dcPackageLengthErr;
  }

  /* Package names must begin with a lowercase alphabetic or numeric
   * character.
   */
  if (!(
    islower(*name) ||
    isdigit(*name)
  )) {
    return dcPackagePrefixErr;
  }
  name++;

  /* Package names must contain only lowercase alphabetic, numeric, or '+',
   * '-' and '.' characters.
   */
  while (*name != '\0')
  {
    if (!(
      islower(*name) ||
      isdigit(*name) ||
      *name == '+' ||
      *name == '-' ||
      *name == '.'
    )) {
      return dcPackageInvalidErr;
    }
    name++;
  }

  return dcNoErr;
}

/**
 * Validate a package version
 *
 * Given a dcVersion object containing some version information, this will
 * determine whether the package version data meets the specification set
 * in Debian Policy 5.6.12.
 *
 * Debian Policy stipulates that:
 * - The epoch is a (generally small) unsigned integer. If it is omitted,
 *   an epoch of zero is assumed and the upstream version may not contain
 *   any ':' characters.
 * - The upstream version may contain alphabetic, numeric and the '.', '+',
 *   '-', ':' and '~' characters.
 * - The Debian revision may contain alphabetic, numeric and the characters
 *   '+', '.' and '~'. If it is omitted (e.g. for Debian native packages),
 *   then the upstream version may not contain any '-' characters.
 *
 * \param[in,out] version A version object to validate
 *
 * \retval dcNoErr if the version appears valid
 * \retval dcParameterErr if the \c version is \c NULL
 *
 * \see "5.6.12 Version", from the Debian Policy Manual:
 * http://www.debian.org/doc/debian-policy/ch-controlfields.html#s-f-Version
 */
dcStatus dc_valid_version(
  const dcVersion *version
) {
  assert(version != NULL);

  if (version == NULL)
    return dcParameterErr;

/*
  if (version->valid)
    return dcNoErr;


 * - The epoch is a (generally small) unsigned integer. If it is omitted,
 *   an epoch of zero is assumed and the upstream version may not contain
 *   any ':' characters.
 * - The upstream version may contain alphabetic, numeric and the '.', '+',
 *   '-', ':' and '~' characters.
 * - The Debian revision may contain alphabetic, numeric and the characters
 *   '+', '.' and '~'. If it is omitted (e.g. for Debian native packages),
 *   then the upstream version may not contain any '-' characters.
 *

*/

  return dcNoErr;
}
