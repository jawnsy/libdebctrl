/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * Debian source package control file parser
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * For more details on how this works, see \ref control.c
 */

#ifndef DEBCTRL_CONTROL_H
#define DEBCTRL_CONTROL_H

#include <debctrl/common.h>
#include <debctrl/parser.h>

struct _dcControlSource
{
  char *name; /**< Name of source package */
};
void dc_control_source_init(
  dcControlSource *source
);
void dc_control_source_clear(
  dcControlSource *source
);

/**
 * A Control metadata parser state object
 *
 * Each dcControl object represents the contents of a given Debian source
 * package metadata file.
 */
struct _dcControl
{
  dcErrorHandler handler; /**< Warning/error handler */

  dcControlSource source; /**< Source package information */
};
/* related methods */
dcControl * dc_control_new(
  void
);
dcStatus dc_control_parse(
  dcControl *control,
  dcParserSection *section
);
dcStatus dc_control_parse_file(
  dcControl *control,
  const char *path
);
void dc_control_free(
  dcControl **ptr
);

#endif /* DEBCTRL_CONTROL_H */
