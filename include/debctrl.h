/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * Debian Control library headers
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * This file provides the public interface for applications using libdebctrl,
 * by importing all of its headers. While this is convenient for developing
 * software using libdebctrl, it may cause a slight reduction in performance
 * during compilation. It should not affect runtime performance.
 *
 * All of the library's API is exposed by using:
 *
 *   \code #include <debctrl.h> \endcode
 *
 * Currently, the following headers are included:
 *  - \ref parser.h
 *  - \ref util.h
 *  - \ref error.h
 */

#ifndef DEBCTRL_H
#define DEBCTRL_H

#include <debctrl/parser.h>
#include <debctrl/util.h>
#include <debctrl/error.h>

#endif /* DEBCTRL_H */

