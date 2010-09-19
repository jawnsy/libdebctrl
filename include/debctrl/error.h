/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * Error handling facilities
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * For more details on how this works, see \ref error.c
 */

#ifndef DEBCTRL_ERROR_H
#define DEBCTRL_ERROR_H

#include <stdarg.h> /* for: va_list */

#include <debctrl/common.h>

typedef enum
{
  dcNoErr,
  dcParameterErr,
  dcMemFullErr,
  dcFileErr,
  dcSyntaxErr
} dcStatus;

/**
 * Provides Error Handling functionality
 *
 * This function provides facilities similar to exceptions, with support for
 * warnings (which will return from its execution context) as well as fatal
 * errors (which may not return).
 */
struct _dcErrorHandler
{
  void (*warn)( /**< Handler for warnings */
    dcParserContext *,
    const char *,
    va_list
  );
  void (*crit)( /**< Handler for critical errors */
    dcParserContext *,
    const char *,
    va_list
  );
};
/* related methods */
dcErrorHandler * dc_error_handler_new(
  void
);
void dc_error_handler_warn(
  dcErrorHandler *handler,
  void (*warn)(
    dcParserContext *,
    const char *,
    va_list
  )
);
void dc_error_handler_crit(
  dcErrorHandler *handler,
  void (*crit)(
    dcParserContext *,
    const char *,
    va_list
  )
);
void dc_vwarn(
  dcErrorHandler *handler,
  dcParserContext *ctx,
  const char *fmt,
  va_list argp
);
void dc_warn(
  dcErrorHandler *handler,
  dcParserContext *ctx,
  const char *fmt,
  ...
);
void dc_vcrit(
  dcErrorHandler *handler,
  dcParserContext *ctx,
  const char *fmt,
  va_list argp
);
void dc_crit(
  dcErrorHandler *handler,
  dcParserContext *ctx,
  const char *fmt,
  ...
);
void dc_error_handler_free(
  dcErrorHandler **ptr
);

#endif /* DEBCTRL_ERROR_H */

