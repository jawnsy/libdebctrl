/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * Generic utilities for libdebctrl
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * This provides utilities for:
 * - string manipulation
 *
 * These utilities are used internally by libdebctrl, and may also be useful
 * externally.
 */

#include <string.h> /* for: strlen, strcat, etc. */

#include <debctrl/util.h>

/**
 * Strip trailing whitespace characters
 *
 * This function removes all trailing whitespace characters from a string,
 * truncating it in-place.
 *
 * \param[in,out] text The string to remove whitespace from
 * \param[in] len The total length of the string
 *
 * \return The pointer to the string (same as \c text)
 *
 * \warning This function will modify the original string by inserting a NUL
 * byte to truncate it. If the original string is needed, be sure to \c strdup
 * the string first.
 *
 * \note This function will remove space (\c 0x20), horizontal tab (\c 0x09),
 * line feed (\c 0xa) and carriage return (\c 0xd) characters.
 */
char * dc_strchomp(
  char *text,
  size_t len
) {
  size_t i;

  assert(text != NULL);
  assert(len > 0);

  for (i = len; i > 0; i--)
  {
    /* we found a non-whitespace character, this one is whitespace */
    if (
      text[i-1] != ' ' &&
      text[i-1] != '\t' &&
      text[i-1] != '\r' &&
      text[i-1] != '\n'
    ) {
      break;
    }
  }

  text[i] = '\0';
  return text;
}

/**
 * Strip leading whitespace characters
 *
 * This function removes all leading whitespace characters from a string, by
 * shifting the string up until the first non-whitespace character is reached.
 *
 * \param[in] text The string to remove whitespace from
 *
 * \return The pointer to the string
 *
 * \warning Although this function does not modify the original string, you
 * should maintain a pointer to the original string to free memory.
 *
 * \note This function will remove space (\c 0x20) and horizontal tab
 * (\c 0x09) characters.
 */
const char * dc_strchug(
  const char *text
) {
  assert(text != NULL);

  while (*text == ' ' || *text == '\t')
  {
    text++;
  }

  return text;
}

/**
 * Duplicate a portion of a string
 *
 * This function duplicates the first \c n bytes of a string, providing a
 * portable replacement for GNU libc's \c strndup function.
 *
 * \param[in,out] text The string to remove whitespace from
 * \param[in] n The number of bytes to duplicate
 *
 * \return A pointer to a new dynamically-allocated string
 *
 * \note If \c _GNU_SOURCE is defined, this function uses strndup to do the
 * heavy lifting.
 */
char * dc_strndup(
  const char *text,
  size_t n
) {
  char *buf;

  assert(text != NULL);
  assert(n > 0);

#ifdef _GNU_SOURCE
  buf = strndup(text, n);
#else /* !_GNU_SOURCE */
  buf = malloc(n+1);
  strncpy(buf, text, n);
  buf[n] = '\0';
#endif /* _GNU_SOURCE */

  return buf;
}

/**
 * Construct a dcString (automatically-expanding string)
 *
 * A dcString is a simple implementation of an automatically-sized string,
 * useful when building large chunks of text. Whenever text is added using
 * \ref dc_string_append, the string will be automatically expanded as needed.
 *
 * \param[in] size The size (in bytes) of the buffer to allocate initially.
 * Using \c 0 will use a default size (\c STRING_INIT_SIZE bytes).
 *
 * \retval NULL if there is a failure to allocate memory (either for dcString
 * or its internal buffer space, dcString::text)
 * \return a dynamically allocated dcString object
 */
dcString * dc_string_new(
  size_t size
) {
  dcString *string = NEW(dcString);

  if (string == NULL)
    return NULL;

  if (size == 0)
    size = STRING_INIT_SIZE;

  string->text = malloc(size);
  if (string->text == NULL)
  {
    free(string);
    return NULL;
  }
  else
  {
    /* put NUL byte at start, for strcpy and friends */
    string->text[0] = '\0';
    string->size = size;
    string->len = 0;
  }
  return string;
}

/**
 * Resize a dcString's internal buffer
 *
 * This function resizes the internal buffer (dcString::text) of a dcString,
 * so that it is at least \c size bytes large. If additional space is needed,
 * it will be allocated in chunks \c INIT_STEP_SIZE large.
 *
 * If \c size is \c 0, the string buffer will be trimmed down to the amount
 * needed for the string, and excess memory will be freed.
 *
 * \param[in] string The dcString to resize
 * \param[in] size The minimum required size (in bytes) of the string buffer
 *
 * \retval 0 if there is a failure to allocate memory
 * \retval 1 if memory trimming/expansion was successful
 *
 * \note An assertion may be raised if the buffer is already large enough
 */
int dc_string_resize(
  dcString *string,
  size_t size
) {
  char *tmp;
  size_t bufsize;

  assert(string != NULL);
  assert(string->text != NULL);

  if (size == 0)
  {
    bufsize = string->len + 1;
  }
  else
  {
    bufsize = string->size;

    while (bufsize < size)
    {
      bufsize += STRING_STEP_SIZE;
    }
  }

  tmp = realloc(string->text, bufsize);
  if (tmp != NULL)
  {
    string->text = tmp;
    string->size = bufsize;
    return 1;
  }

  return 0;
}

/**
 * Append text to a dcString
 *
 * This method appends text to the end of a dcString, expanding the internal
 * buffer if necessary (using \ref dc_string_resize).
 *
 * \param[in] string A dcString for which text will be appended
 * \param[in] text A \c NULL terminated C string to append
 *
 * \note If there is a failure to allocate memory on resize, the string will
 * be truncated and terminated with an ellipsis ("...") to indicate this case.
 */
void dc_string_append(
  dcString *string,
  const char *text
) {
  size_t len; /* total space required (existing length + text length) */

  assert(string != NULL);
  assert(string->text != NULL);
  assert(text != NULL);

  len = strlen(text);
  /* ensure we have enough space in our buffer, expand if needed */
  if ((string->len + len) >= string->size)
  {
    if (dc_string_resize(string, string->len + len + 1) == 0)
    {
      /* insufficient memory for new string, copy as much of it as possible */
      if ((string->size - string->len) > 0)
      {
        /* leave 4 bytes for "..." + NUL at the end, to indicate truncation */
        strncat(string->text, text, string->size - string->len - 4);
        string->len = string->size - 1;
      }
      string->text[string->len - 3] = '.';
      string->text[string->len - 2] = '.';
      string->text[string->len - 1] = '.';
      string->text[string->len    ] = '\0';
      return;
    }
  }

  /* we are guaranteed to have sufficient space */
  strcat(string->text, text);
  string->len += len;
}

/**
 * Append a character to a dcString
 *
 * This method appends a single character to the end of a dcString, expanding
 * the internal buffer if necessary (using \ref dc_string_resize).
 *
 * \param[in] string A dcString for which text will be appended
 * \param[in] c A single character to append
 *
 * \note For short strings, it's more efficient to call this method multiple
 * times, since \ref dc_string_append jumps to \c strlen and does a bunch of
 * other stuff.
 */
void dc_string_append_c(
  dcString *string,
  char c
) {
  assert(string != NULL);
  assert(string->text != NULL);

  /* ensure we have enough space in our buffer, expand if needed */
  if ((string->len + 1) >= string->size)
  {
    if (dc_string_resize(string, string->len + 2) == 0)
    {
      /* insufficient memory for new character, give up */
      return;
    }
  }

  /* we are guaranteed to have sufficient space */
  string->text[string->len++] = c;
  string->text[string->len]   = '\0';
}

/**
 * Destroy a dcString
 *
 * Given a dcString that was allocated by \ref dc_string_new, this will free
 * internally-allocated memory before destroying the string object itself.
 *
 * \param[in,out] ptr The address of a pointer to a dcString
 *
 * \note The pointer will be set to \c NULL after memory is freed.
 */
void dc_string_free(
  dcString **ptr
) {
  assert(ptr != NULL);
  assert(*ptr != NULL);

  free((*ptr)->text);

  free(*ptr);
  *ptr = NULL;
}
