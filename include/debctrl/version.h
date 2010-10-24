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
 * For more details on how this works, see \ref version.c
 */

#ifndef DEBCTRL_VERSION_H
#define DEBCTRL_VERSION_H

#include <debctrl/common.h>

/**
 * Representation of a package version
 *
 * This structure provides a representation of the components of a package
 * version, namely the:
 *  - Package epoch
 *  - Debian package revision
 *  - Upstream package version
 */
struct _dcVersion
{
  unsigned long epoch; /**< Epoch number */
  char *version;  /**< Upstream version */
  char *revision; /**< Debian package revision */
};
/* related methods */
dcVersion * dc_version_new(
  void
);
dcStatus dc_version_set(
  dcVersion *version,
  const char *vstring
);
void dc_version_free(
  dcVersion **ptr
);

#endif /* DEBCTRL_VERSION_H */
