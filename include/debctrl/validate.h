/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * Debian package information validation
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * For more details on how this works, see \ref validate.c
 */

#ifndef DEBCTRL_VALIDATE_H
#define DEBCTRL_VALIDATE_H

#include <debctrl/common.h>

dcStatus dc_valid_package(
  const char *name
);

#endif /* DEBCTRL_VALIDATE_H */
