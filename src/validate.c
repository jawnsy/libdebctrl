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
  /* Validate the package name, according to Policy 5.6.1 */

  /* Package names must be at least two characters long */
  if (name == NULL || *name == '\0' || *(name+1) == '\0')
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
