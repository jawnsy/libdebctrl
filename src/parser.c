/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * RFC822 control file parser
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * Processing Debian's package metadata ("control") files occurs in two steps:
 * -# Text is parsed into a data structure representation (syntax)
 * -# Specific data is extracted from the data structure (semantics)
 *
 * The routines in this file constitute the first step: they read information
 * from an input file into a data structure, which can then be modified and
 * written back to disk. This parser is "dumb" in that it cannot decipher the
 * \em semantic meaning of the fields and data it encounters.
 *
 * \section format File Format Specification
 * \par Introduction to RFC822
 * RFC822 is a specification concerned primarily with providing an extensible
 * and easily maintainable format that is also machine-readable. This parser
 * works based on the semantics presented in the Debian Policy Manual chapter
 * entitled "Control files and their fields".
 *
 * \par Sections and Blocks
 * \ref dcParserSection objects correspond to individual
 * "paragraphs" as defined by the Policy. In the \c debian/control file, these
 * sections are used to separate metadata for a source package and all of the
 * binary packages it produces.
 * \par
 * Each section contains various field-value pairs, herein described by the
 * \ref dcParserBlock object.
 *
 * \par Data Chunks
 * The values for each block become \ref dcParserChunk objects, which may be
 * marked \c Mergeable or \c Fixed depending on the number of spaces at the
 * start of each continuation section.
 * \par
 * One space means that the line can be combined with the previous section,
 * whereas two spaces means the line is considered fixed-formatting and must
 * be reproduced exactly as-is.
 * \par
 * These properties may be ignored later under certain conditions, such as in
 * the \c Depends sections of the \c debian/control file.
 *
 * \par Comments
 * Comments in control files are partially supported, in the sense that their
 * presence will not cause an error. These are simply removed while gathering
 * input. Be careful not to use comments in files other than \c debian/control
 * and any other files as indicated in the Debian Policy Manual, because this
 * library does not emit warnings about them.
 *
 * \bug All error messages are in English and are not internationalized
 *
 * \see "Control files and their fields", from the Debian Policy Manual:
 * http://www.debian.org/doc/debian-policy/ch-controlfields.html
 * \see RFC-822: Format of ARPA Messages http://www.faqs.org/rfcs/rfc822.html
 */

#include <string.h>   /* for: strdup */
#include <strings.h>  /* for: strcasecmp */
#include <stdio.h>    /* for: fopen, etc. */
#include <errno.h>    /* for: errno */

#include <debctrl/parser.h>
#include <debctrl/error.h>

/**
 * Construct a Parser Chunk
 *
 * For details on the structure and its fields, see \ref dcParserChunk
 *
 * \param[in] text A chunk of textual data. The text is internally copied, so
 * the source string may be freed.
 *
 * \retval NULL if there is a failure to allocate memory
 * \return a dynamically allocated dcParserChunk object
 *
 * \note If \c text is \c NULL, the chunk will have type \c CHUNK_EMPTY.
 *
 * \note If \c text is not \c NULL, the chunk will have type \c CHUNK_MERGE.
 */
dcParserChunk * dc_parser_chunk_new(
  const char *text
) {
  dcParserChunk *chunk = NEW(dcParserChunk);

  if (chunk == NULL)
    return NULL;

  if (text == NULL)
  {
    chunk->text = NULL;
    chunk->type = CHUNK_EMPTY;
  }
  else
  {
    chunk->text = strdup(text);
    chunk->type = CHUNK_MERGE;
  }

  chunk->next = NULL;
  chunk->prev = NULL;

  return chunk;
}

/**
 * Destroy a Parser Chunk
 *
 * Given a \ref dcParserChunk that was allocated by \ref dc_parser_chunk_new,
 * this will automatically free internally-allocated memory before destroying
 * the chunk itself.
 *
 * \param[in,out] ptr The address of a pointer to a Parser Chunk
 *
 * \note The pointer will be set to \c NULL after memory is freed.
 */
void dc_parser_chunk_free(
  dcParserChunk **ptr
) {
  assert(ptr != NULL);
  assert(*ptr != NULL);

  if ((*ptr)->text != NULL)
    free((*ptr)->text);

  free(*ptr);

  *ptr = NULL;
}

/**
 * Construct a Parser Block
 *
 * For details on the structure and its fields, see \ref dcParserBlock
 *
 * \param[in] name The field name associated with the given Block. The text
 * data is internally copied, so the source string may be freed.
 *
 * \retval NULL if there is a failure to allocate memory
 * \return a dynamically allocated dcParserBlock object
 */
dcParserBlock * dc_parser_block_new(
  const char *name
) {
  dcParserBlock *block = NEW(dcParserBlock);

  if (block == NULL)
    return NULL;

  block->name = (name) ? strdup(name) : NULL;

  block->head = NULL;
  block->tail = NULL;

  block->next = NULL;

  return block;
}

/**
 * Append a Parser Chunk to a Parser Block
 *
 * This routine will append a given \ref dcParserChunk object to the end of
 * the \ref dcParserBlock.
 *
 * \param[in,out] block A pointer to a Parser Block
 * \param[in,out] chunk A pointer to a Parser Chunk
 */
void dc_parser_block_append(
  dcParserBlock *block,
  dcParserChunk *chunk
) {
  assert(block != NULL);
  assert(chunk != NULL);

  if (block->head == NULL)
  {
    block->head = chunk;
    block->tail = chunk;
  }
  else
  {
    chunk->prev = block->tail;
    block->tail->next = chunk;
    block->tail = chunk;
  }
}

/**
 * Prepend a Parser Chunk to a Parser Block
 *
 * This routine will prepend a given \ref dcParserChunk object to the start of
 * the \ref dcParserBlock.
 *
 * \param[in,out] block A pointer to a Parser Block
 * \param[in,out] chunk A pointer to a Parser Chunk
 */
void dc_parser_block_prepend(
  dcParserBlock *block,
  dcParserChunk *chunk
) {
  assert(block != NULL);

  if (block->head == NULL)
  {
    block->head = chunk;
    block->tail = chunk;
  }
  else
  {
    chunk->next = block->head;
    block->head->prev = chunk;
    block->head = chunk;
  }
}

/**
 * Delete a Parser Chunk from a Parser Block
 *
 * This routine is similar to \ref dc_parser_chunk_free, except it will remove
 * a given \ref dcParserChunk from its \ref dcParserBlock first.
 *
 * \param[in,out] block A pointer to a Parser Block
 * \param[in,out] chunk The address of a pointer to a Parser Chunk
 */
void dc_parser_block_delete(
  dcParserBlock *block,
  dcParserChunk **chunk
) {
  assert(block != NULL);
  assert(block->head != NULL);
  assert(chunk != NULL);
  assert(*chunk != NULL);

  /* if the chunk is the head, the new head is ->next */
  if ((*chunk)->prev == NULL)
  {
    block->head = (*chunk)->next;
  }
  /* the last chunk should point to the next chunk */
  else
  {
    (*chunk)->prev->next = (*chunk)->next;
  }

  /* if the chunk is the tail, the new tail is ->prev */
  if ((*chunk)->next == NULL)
  {
    block->tail = (*chunk)->prev;
  }
  /* the next chunk should point to the previous one */
  else
  {
    (*chunk)->next->prev = (*chunk)->prev;
  }

  dc_parser_chunk_free(chunk);
}

/**
 * Return contents of a Parser Block as a dcString
 *
 * This routine will flatten a given \ref dcParserBlock object into a
 * \ref dcString, which can be converted to a normal string to display it on
 * the console or write it out to file.
 *
 * \param[in] block A pointer to a Parser Block
 *
 * \note This routine uses dcString to ensure the buffer is automatically
 * resized (expanded) as necessary.
 *
 * \deprecated This routine is deprecated and will be replaced in a subsequent
 * release.
 */
dcString * dc_parser_block_string(
  dcParserBlock *block
) {
  dcParserChunk *chunk;
  dcString *buf;

  assert(block != NULL);
  assert(block->head != NULL);

  buf = dc_string_new(0);
  if (buf == NULL)
    return NULL;

  chunk = block->head;

  /* The head chunk is next to the field name */
  dc_string_append(buf, block->name);
  dc_string_append_c(buf, ':');
  dc_string_append_c(buf, ' ');
  dc_string_append(buf, chunk->text);
  dc_string_append_c(buf, '\n');

  while ((chunk = chunk->next) != NULL)
  {
    if (chunk->type == CHUNK_EMPTY)
    {
      dc_string_append_c(buf, ' ');
      dc_string_append_c(buf, '.');
      dc_string_append_c(buf, '\n');
    }
    else
    {
      dc_string_append_c(buf, ' ');
      if (chunk->type == CHUNK_FIXED)
      {
        dc_string_append_c(buf, ' ');
      }
      dc_string_append(buf, chunk->text);
      dc_string_append_c(buf, '\n');
    }
  }

  /* trim down and return string */
  dc_string_resize(buf, 0);
  return buf;
}

/**
 * Destroy a Parser Block
 *
 * Given a \ref dcParserBlock that was allocated by \ref dc_parser_block_new,
 * this will automatically free internally-allocated memory before destroying
 * the block itself.
 *
 * \param[in,out] ptr The address of a pointer to a Parser Block
 *
 * \note The pointer will be set to \c NULL after memory is freed.
 *
 * \note Using this will also destroy internally-stored chunks; if you wish
 * to preserve these, set the \c head and \c tail to \c NULL first.
 */
void dc_parser_block_free(
  dcParserBlock **ptr
) {
  assert(ptr != NULL);
  assert(*ptr != NULL);

  if ((*ptr)->name != NULL)
    free((*ptr)->name);

  if ((*ptr)->head != NULL)
  {
    dcParserChunk *chunk;
    dcParserChunk *next;

    chunk = (*ptr)->head;
    while (chunk != NULL)
    {
      next = chunk->next;

      dc_parser_chunk_free(&chunk);

      chunk = next;
    }
    (*ptr)->head = NULL;
    (*ptr)->tail = NULL;
  }

  free(*ptr);
  *ptr = NULL;
}

/**
 * Construct a Parser Section
 *
 * For details on the structure and its fields, see \ref dcParserSection
 *
 * \retval NULL if there is a failure to allocate memory
 * \return a dynamically allocated dcParserSection object
 */
dcParserSection * dc_parser_section_new(
  void
) {
  dcParserSection *section = NEW(dcParserSection);

  if (section == NULL)
    return NULL;

  /* set everything to NULL so we don't get confused */
  section->head = NULL;
  section->tail = NULL;
  section->next = NULL;

  return section;
}

/**
 * Append a Parser Block to a Parser Section
 *
 * This routine will append a given \ref dcParserBlock object to the end of
 * the \ref dcParserSection.
 *
 * \param[in,out] section A pointer to a Parser Section
 * \param[in,out] block A pointer to a Parser Block
 */
void dc_parser_section_append(
  dcParserSection *section,
  dcParserBlock *block
) {
  assert(section != NULL);
  assert(block != NULL);

  if (section->head == NULL)
  {
    section->head = block;
    section->tail = block;
  }
  else
  {
    block->prev = section->tail;
    section->tail->next = block;
    section->tail = block;
  }
}

/**
 * Find a Parser Block (by name) in a section
 *
 * This routine searches a given \ref dcParserSection for a block, based on
 * the block name.
 *
 * \param[in,out] section A pointer to a Parser Section
 * \param[in] field A field name to look for
 *
 * \retval NULL if no matching block was found
 * \return A dcParserBlock with a matching \c field name
 *
 * \note Debian Policy 5.1 stipulates that field names are not case sensitive.
 */
dcParserBlock * dc_parser_section_find(
  dcParserSection *section,
  const char *field
) {
  dcParserBlock *block;

  assert(section != NULL);
  assert(field != NULL);

  block = section->head;
  while (block != NULL)
  {
    if (strcasecmp(block->name, field) == 0)
      return block;

    block = block->next;
  }

  return NULL;
}

/**
 * Destroy a Parser Section
 *
 * Given a \ref dcParserSection that was allocated by \ref dc_parser_section_new,
 * this will automatically free internally-allocated memory before destroying
 * the section itself.
 *
 * \param[in,out] ptr The address of a pointer to a Parser Section
 *
 * \note The pointer will be set to \c NULL after memory is freed.
 *
 * \note Using this will also destroy internally-stored blocks; if you wish
 * to preserve these, set the \c head and \c tail to \c NULL first.
 */
void dc_parser_section_free(
  dcParserSection **ptr
) {
  assert(ptr != NULL);
  assert(*ptr != NULL);

  if ((*ptr)->head != NULL)
  {
    dcParserBlock *block;
    dcParserBlock *next;

    block = (*ptr)->head;
    while (block != NULL)
    {
      next = block->next;

      dc_parser_block_free(&block);

      block = next;
    }
    (*ptr)->head = NULL;
    (*ptr)->tail = NULL;
  }

  free(*ptr);
  *ptr = NULL;
}

/**
 * Construct a Parser instance
 *
 * For details on the structure and its fields, see \ref dcParser
 *
 * \retval NULL if there is a failure to allocate memory
 * \return a dynamically allocated dcParser object
 */
dcParser * dc_parser_new(
  void
) {
  dcParser *parser = NEW(dcParser);

  if (parser == NULL)
    return NULL;

  parser->head = NULL;
  parser->ctx.line = 0;
  parser->ctx.path = NULL;

  /* set up default error handler */
  parser->handler = dc_error_handler_new();

  return parser;
}

/**
 * Process a textual "chunk" of data
 *
 * Continuation lines are internally identified as a "chunk" of data, whether
 * they are fixed-format or mergeable with the previous line.
 *
 * This internal helper method processes a chunk and identifies it as "fixed",
 * "mergeable" or "empty" (see \ref dcParserChunkType for details), and adds
 * the chunk to the last open \ref dcParserBlock.
 *
 * \param[in,out] parser A pointer to a Parser instance
 * \param[in] line A line of textual data (a chunk)
 *
 * \note Any problems manipulating the chunk will be reported using a dcStatus
 * code, which should then be propagated to the external calling function.
 */
static dcStatus dc_parse_chunk(
  dcParser *parser,
  const char *line
) {
  dcParserChunk *chunk;

  assert(parser != NULL);
  assert(line != NULL);

  if (parser == NULL || line == NULL)
    return dcParameterErr;

  assert(parser->tail != NULL);

  /* if parser->tail->tail is NULL, then no blocks have been opened yet */
  if (parser->tail->tail == NULL)
  {
    dc_crit(parser->handler, &parser->ctx, _("Attempted to continue previous "
      "statement, however, none have been opened yet."));
    return dcSyntaxErr;
  }

  /* this probably doesn't matter...

  if (line[0] == '\t' || line[1] == '\t')
  {
    dc_warn(parser->handler, &parser->ctx, "Hard tabs in continuations will "
      "be transformed into spaces");
  }

  */

  if (line[1] == '.')
  {
    /* check that the full stop is the only thing on this line */
    if (line[2] == '\0')
    {
      chunk = dc_parser_chunk_new(NULL);
    }
    else
    {
      dc_crit(parser->handler, &parser->ctx, _("Lines beginning with '.' are "
        "reserved for future use (Sec. 5.6.13)"));
      return dcSyntaxErr;
    }
  }
  else if (line[1] == ' ' || line[1] == '\t')
  {
    chunk = dc_parser_chunk_new(line + 2);
    chunk->type = CHUNK_FIXED;
  }
  else
  {
    chunk = dc_parser_chunk_new(line + 1);
  }

  if (chunk == NULL)
    return dcMemFullErr;

  /* shallow copy of the current parsing context */
  chunk->ctx = parser->ctx;

  dc_parser_block_append(parser->tail->tail, chunk);

  return dcNoErr;
}

/**
 * Process a textual "block" of data
 *
 * Lines beginning a new field are internally identified as a "block" of data,
 * which may optionally have some data (the "chunks"). The text between the
 * ":" and end-of-line is treated as a fixed-width \ref dcParserChunk (but it
 * may also be empty, in case a field is on a line by itself).
 *
 * \param[in,out] parser A pointer to a Parser instance
 * \param[in] line A line of textual data (a block)
 *
 * \note Any problems manipulating the block will be reported using a dcStatus
 * code, which should then be propagated to the external calling function.
 */
static dcStatus dc_parse_block(
  dcParser *parser,
  const char *line
) {
  char *field;
  char *text;
  dcParserBlock *block = NULL;
  dcParserChunk *chunk = NULL;

  assert(parser != NULL);
  assert(line != NULL);

  if (parser == NULL || line == NULL)
    return dcParameterErr;

  /* this code copies the line and calls it the "field", then advances the
   * "text" pointer until the colon is found, where it replaces it with \0.
   *
   * if successful, the first part of "field" before the first \0 will contain
   * the field name; the segment that 'text' points to will contain the text.
   */
  field = strdup(line);
  if (field == NULL)
    return dcMemFullErr;
  text = field;

  /* scan the line for a ":", replacing it with a NUL byte if found */
  while (*text != '\0')
  {
    if (*text == ':')
    {
      *text++ = '\0';
      break;
    }
    text++;
  }

  /* if we have an empty text section, then there was no ":" */
  if (*text == '\0' && *(text-1) != '\0')
  {
    dc_crit(parser->handler, &parser->ctx, _("Expected pseudoheader/data "
      "pair (Sec. 5.1); if continuing a previous line, add a space"));
    free(field);
    return dcSyntaxErr;
  }

  text = (char *) dc_strchug(text);

  /* ensure this block is not a duplicate */
  block = dc_parser_section_find(parser->tail, field);
  if (block != NULL)
  {
    dc_warn(parser->handler, &parser->ctx, _("Duplicate field names are not "
      "permitted (Sec. 5.1), contents will be merged together"));
  }
  else
  {
    block = dc_parser_block_new(field);
    if (block == NULL)
      return dcMemFullErr;

    assert(parser->tail != NULL);

    dc_parser_section_append(parser->tail, block);
  }

  if (*text == '\0')
  {
    chunk = dc_parser_chunk_new(NULL);
    if (chunk == NULL)
      return dcMemFullErr;
    chunk->type = CHUNK_EMPTY;
  }
  else
  {
    chunk = dc_parser_chunk_new(text);
    if (chunk == NULL)
      return dcMemFullErr;
    chunk->type = CHUNK_FIXED;
  }

  /* shallow copy of the current parsing context */
  chunk->ctx = parser->ctx;

  dc_parser_block_append(block, chunk);

  free(field);

  return dcNoErr;
}

/**
 * Process a file into Parser data structures
 *
 * This method opens a file and processes it into an internal representation.
 *
 * For details on the structures their fields, see: \ref dcParserSection,
 * \ref dcParserBlock, \ref dcParserChunk and \ref dcParser.
 *
 * \param[in,out] parser A pointer to a Parser instance
 * \param[in] path The path to the file to open
 *
 * \note Any problems manipulating the file will be reported via the dcParser
 * \link error.c error handler interface \endlink, and the status indication
 * will be returned (as a dcStatus).
 */
dcStatus dc_parser_read_file(
  dcParser *parser,
  const char *path
) {
  FILE *fp;
  char *line = NULL;
  size_t size = 0;
  ssize_t len;
  dcStatus rc; /* return status of chunk/block parser */
  dcParserSection *section = NULL;

  assert(parser != NULL);
  assert(parser->head == NULL);
  assert(path != NULL);

  if (parser == NULL || parser->head != NULL || path == NULL)
    return dcParameterErr;

  parser->ctx.path = strdup(path);

  fp = fopen(path, "r");
  if (fp == NULL)
  {
    dc_crit(parser->handler, NULL, _("Can't open file '%s': %s"),
      path, strerror(errno));
    return dcFileErr;
  }

  section = dc_parser_section_new();
  if (section == NULL)
    return dcMemFullErr;
  dc_parser_append(parser, section);

  while ((len = getline(&line, &size, fp)) != -1)
  {
    rc = dc_parser_read_line(parser, line, len);

    /* if there were parsing errors, abort */
    if (rc != dcNoErr)
      break;
  }

  free(line);
  fclose(fp);

  return rc;
}

/**
 * Process a line into Parser data structures
 *
 * This method parses a single line into an internal representation.
 *
 * For details on the structures their fields, see: \ref dcParserSection,
 * \ref dcParserBlock, \ref dcParserChunk and \ref dcParser.
 *
 * \param[in,out] parser A pointer to a Parser instance
 * \param[in,out] line A pointer to the text to be parsed
 * \param[in] len The length of the string
 *
 * \note Any problems manipulating the file will be reported via the dcParser
 * \link error.c error handler interface \endlink, and the status indication
 * will be returned (as a dcStatus).
 */
dcStatus dc_parser_read_line(
  dcParser *parser,
  char *line,
  size_t len
) {
  dcStatus rc; /* return status of chunk/block parser */
  dcParserSection *section;

  assert(parser != NULL);
  assert(line != NULL);

  if (parser == NULL || parser->head == NULL || line == NULL)
    return dcParameterErr;

  parser->ctx.line++;

  section = parser->tail;

  /* XXX: Ignore comments completely */
  if (line[0] == '#')
    return dcNoErr;

  /* remove trailing whitespace */
  dc_strchomp(line, len);

  /* If there is a byte of whitespace, the line can be fixed, mergeable or
   * empty. Otherwise, it is a new block (or some garbage in the file).
   */

  /* If the line is empty, a new section is starting */
  if (line[0] == '\0')
  {
    /* check if the current section is empty */
    if (section->tail == NULL)
    {
      dc_warn(parser->handler, &parser->ctx, _("Multiple blank lines will "
        "be transformed into a single blank line"));
    }
    else
    {
      section = dc_parser_section_new();
      if (section == NULL)
        return dcMemFullErr;
      dc_parser_append(parser, section);
    }
    return dcNoErr;
  }

  if (line[0] == ' ' || line[0] == '\t')
    rc = dc_parse_chunk(parser, line);
  else
    rc = dc_parse_block(parser, line);

  return rc;
}

/**
 * Append a Parser Block to a Parser instance
 *
 * This routine will append a given \ref dcParserSection object to the end of
 * the \ref dcParser's internal list.
 *
 * \param[in,out] parser A pointer to a Parser instance
 * \param[in] section A pointer to a Parser Section
 */
void dc_parser_append(
  dcParser *parser,
  dcParserSection *section
) {
  dcParserSection *node;

  assert(parser != NULL);
  assert(section != NULL);

  node = parser->head;
  if (node == NULL)
  {
    parser->head = section;
  }
  else
  {
    while (node->next != NULL)
    {
      node = node->next;
    }
    node->next = section;
  }
  parser->tail = section;
}

/**
 * Destroy a Parser instance
 *
 * Given a Parser instance that was allocated by \ref dc_parser_new, this will
 * automatically free internally-allocated memory before destroying the parser
 * itself.
 *
 * \param[in,out] ptr The address of a pointer to a Parser
 *
 * \note The pointer will be set to \c NULL after memory is freed.
 *
 * \note Using this will also destroy internally-stored sections; if you wish
 * to preserve these, set the \c head to \c NULL first.
 */
void dc_parser_free(
  dcParser **ptr
) {
  assert(ptr != NULL);
  assert(*ptr != NULL);

  if ((*ptr)->ctx.path != NULL)
    free((*ptr)->ctx.path);

  if ((*ptr)->handler != NULL)
    dc_error_handler_free(&((*ptr)->handler));

  if ((*ptr)->head != NULL)
  {
    dcParserSection *section;
    dcParserSection *next;

    section = (*ptr)->head;
    while (section != NULL)
    {
      next = section->next;

      dc_parser_section_free(&section);

      section = next;
    }
    (*ptr)->head = NULL;
  }

  free(*ptr);
  *ptr = NULL;
}
