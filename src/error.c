/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * Error handling facilities
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * \par Handler functions
 * Handler functions must follow the prototype:
 *
 * \code
 *  struct dcParserContext *ctx, const char *fmt, va_list argp
 * \endcode
 *
 * - \c ctx, the current Parser Context. It may be \c NULL (e.g. for errors
 *   involving file permissions)
 * - \c fmt, the \c printf-style format string for the error
 * - \c argp, the variable argument list (standard \c va_list)
 */

#include <stdio.h> /* for: printf, stderr, etc. */

#include <debctrl/error.h>
#include <debctrl/parser.h> /* for: dcParser and friends */

/* default error handling routines */
static void dc_emit_warning(
  dcParserContext *ctx,
  const char *fmt,
  va_list argp
) {
  assert(fmt != NULL);

  fprintf(stderr, _("warning: "));
  vfprintf(stderr, fmt, argp);

  if (ctx != NULL)
  {
    fprintf(stderr, _(" at %s line %u"), ctx->path, ctx->line);
  }

  fprintf(stderr, "\n");
}
static void dc_emit_error(
  dcParserContext *ctx,
  const char *fmt,
  va_list argp
) {
  assert(fmt != NULL);

  fprintf(stderr, _("critical error: "));
  vfprintf(stderr, fmt, argp);

  if (ctx != NULL)
  {
    fprintf(stderr, _(" at %s line %u"), ctx->path, ctx->line);
  }

  fprintf(stderr, "\n");
}

/**
 * Construct an Error Handler state object
 *
 * For details on the structure and its fields, see \ref dcErrorHandler
 *
 * \retval NULL if there is a failure to allocate memory
 * \return a dynamically allocated dcErrorHandler object
 *
 * \note Unless otherwise set, built-in error handlers will be used.
 */
dcErrorHandler * dc_error_handler_new(
  void
) {
  dcErrorHandler *handler = NEW(dcErrorHandler);

  if (handler == NULL)
    return NULL;

  dc_error_handler_init(handler);

  return handler;
}

void dc_error_handler_init(
  dcErrorHandler *handler
) {
  assert(handler != NULL);

  handler->warn = &dc_emit_warning;
  handler->crit = &dc_emit_error;
}

/**
 * Modify the warning handler function
 *
 * This routine sets the warning handler for a given Error Handler instance.
 *
 * \param[in] handler Pointer to an Error Handler state object
 * \param[in] warn Callback to warning handler routine
 *
 * \note If \c warn is the special value \c NULL, the default warning handler
 * will be restored
 */
void dc_error_handler_warn(
  dcErrorHandler *handler,
  void (*warn)(
    dcParserContext *,
    const char *,
    va_list
  )
) {
  assert(handler != NULL);

  if (warn == NULL)
    handler->warn = &dc_emit_warning;
  else
    handler->warn = warn;
}

/**
 * Modify the critical error handler function
 *
 * This routine sets the critical error handler for a given Error Handler
 * instance.
 *
 * \param[in] handler Pointer to an Error Handler state object
 * \param[in] crit Callback to critical error handler routine
 *
 * \note If \c crit is the special value \c NULL, the default critical error
 * handler will be restored
 */
void dc_error_handler_crit(
  dcErrorHandler *handler,
  void (*crit)(
    dcParserContext *,
    const char *,
    va_list
  )
) {
  assert(handler != NULL);

  if (crit == NULL)
    handler->crit = &dc_emit_error;
  else
    handler->crit = crit;
}

/**
 * Emit a warning (using a \c va_list)
 *
 * This routine provides identical functionality to \ref dc_warn, but is used
 * with variable argument lists (the \c va_list parameter). It is useful when
 * you want to provide a wrapper around the warning facility (e.g. write to a
 * log file and pass the warning to the application handler).
 *
 * \param[in] handler A dcErrorHandler state object
 * \param[in] ctx The originating context of the warning, if applicable. For
 * issues not directly related to a specific line number, this may be \c NULL.
 * \param[in] fmt A \c printf-compatible format string
 * \param[in] argp A standard \c va_list of arguments
 */
void dc_vwarn(
  dcErrorHandler *handler,
  dcParserContext *ctx,
  const char *fmt,
  va_list argp
) {
  assert(handler != NULL);
  assert(handler->warn != NULL);
  assert(fmt != NULL);

  (*handler->warn)(ctx, fmt, argp);
}

/**
 * Emit a warning
 *
 * This routine emits a warning using the \c warn handler associated with the
 * given dcErrorHandler object.
 *
 * Example:
 * \code
 * dc_warn(handler, NULL, "Could not read file: %s", error);
 * \endcode
 *
 * \param[in] handler A dcErrorHandler state object
 * \param[in] ctx The originating context of the warning, if applicable. For
 * issues not directly related to a specific line number, this may be \c NULL.
 * \param[in] fmt A \c printf-compatible format string
 */
void dc_warn(
  dcErrorHandler *handler,
  dcParserContext *ctx,
  const char *fmt,
  ...
) {
  va_list argp;

  va_start(argp, fmt);
  dc_vwarn(handler, ctx, fmt, argp);
  va_end(argp);
}

/**
 * Emit a critical error message (using a \c va_list)
 *
 * This routine provides identical functionality to \ref dc_crit, but is used
 * with variable argument lists (the \c va_list parameter). It is useful when
 * you want to provide a wrapper around the error (e.g. write to a log file
 * and pass the error message to the application handler).
 *
 * \param[in] handler A dcErrorHandler state object
 * \param[in] ctx The originating context of the error, if applicable. For
 * issues not directly related to a specific line number, this may be \c NULL.
 * \param[in] fmt A \c printf-compatible format string
 * \param[in] argp A standard \c va_list of arguments
 */
void dc_vcrit(
  dcErrorHandler *handler,
  dcParserContext *ctx,
  const char *fmt,
  va_list argp
) {
  assert(handler != NULL);
  assert(handler->crit != NULL);
  assert(fmt != NULL);

  (*handler->crit)(ctx, fmt, argp);
}

/**
 * Emit a critical error message
 *
 * This routine emits a critical error message using the \c crit handler
 * associated with the given dcErrorHandler object.
 *
 * Example:
 * \code
 * dc_crit(handler, NULL, "Failed to execute program: %s", app);
 * \endcode
 *
 * \param[in] handler A dcErrorHandler state object
 * \param[in] ctx The originating context of the error, if applicable. For
 * issues not directly related to a specific line number, this may be \c NULL.
 * \param[in] fmt A \c printf-compatible format string
 */
void dc_crit(
  dcErrorHandler *handler,
  dcParserContext *ctx,
  const char *fmt,
  ...
) {
  va_list argp;

  va_start(argp, fmt);
  dc_vcrit(handler, ctx, fmt, argp);
  va_end(argp);
}

/**
 * Destroy an Error Handler state object
 *
 * Given an Error Handler that was allocated by \ref dc_error_handler_new,
 * this will automatically free internally-allocated memory before destroying
 * the block itself.
 *
 * \param[in,out] ptr The address of a pointer to an Error Handler
 *
 * \note The pointer will be set to \c NULL after memory is freed.
 */
void dc_error_handler_free(
  dcErrorHandler **ptr
) {
  assert(ptr != NULL);
  assert(*ptr != NULL);

  free(*ptr);
  *ptr = NULL;
}
