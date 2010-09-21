/***************************************************************************
** The libdebctrl library, this package and its contents (unless explicitly
** noted otherwise) are licensed under the MIT/X11 License (see the included
** LICENSE file for copyright information, full terms and conditions).
****************************************************************************/

/** \file
 * RFC822 control file parser
 * \author Jonathan Yu <jawnsy@cpan.org>
 *
 * Internally, various flags determine the behaviour of data specified in
 * Debian metadata (so-called "control") files.
 *
 * Briefly:
 * \li \c CHUNK_EMPTY means the chunk contains no data
 * \li \c CHUNK_MERGE means the chunk can be merged with the previous one
 * \li \c CHUNK_FIXED means the chunk has fixed formatting
 *
 * For more details on how this works, see \ref parser.c
 */

#ifndef DEBCTRL_PARSER_H
#define DEBCTRL_PARSER_H

#include <debctrl/common.h>
#include <debctrl/util.h>   /* for: dcString */
#include <debctrl/error.h>  /* for: dcStatus */

/**
 * This enumeration represents the type of data chunk, based on whether it
 * is a continuation line or whether it is fixed-formatting.
 *
 * \par Mergeable vs Fixed-Width
 * If a continuation line for a field begins with a single space or tab prefix,
 * then it is considered a mergeable continuation of the previous block. This
 * property means that subsequent lines which are also marked mergeable can be
 * considered the same logical line.
 *
 * \par Empty Chunks
 * Empty chunks represent a continuation line which contains a single full stop
 * "." character, so that the output is rendered as blank line. This ensures
 * that blank lines are not accidentally misinterpreted as a new paragraph.
 */
enum dcParserChunkType
{
  /**
   * This chunk contains no data. Where the chunk is empty, its \c text field
   * should also be set to \c NULL.
   */
  CHUNK_EMPTY,

  /**
   * This chunk is a continuation which may be merged with the data contained
   * in the previous chunk.
   */
  CHUNK_MERGE,

  /**
   * This chunk is a fixed-format line which must not be merged with the data
   * contained in the previous chunk.
   */
  CHUNK_FIXED
};

/**
 * Parser Context
 *
 * A dcParserContext object provides a simple (filename, line number) tuple
 * that can be used to identify the originating context of a given block or
 * error message.
 *
 * It is used particularly to provide useful debugging output; eg., "there is
 * an unknown block in \c debian/control at line 30."
 */
struct _dcParserContext
{
  char *path; /**< Path to filename */
  unsigned int line; /**< Source line number of this chunk */
};

/**
 * A single line (chunk) of textual data
 *
 * As each line of data is parsed, it is organized into chunks of data. These
 * chunks are flagged to determine whether they should be merged with previous
 * lines or kept as-is (for pre-formatted data).
 */
struct _dcParserChunk
{
  char *text; /**< A chunk of data from the parsed file */

  enum dcParserChunkType type; /**< Type of this chunk */

  dcParserContext ctx; /**< Originating context of this chunk */

  dcParserChunk *next; /**< Next chunk in this block */
  dcParserChunk *prev; /**< Previous chunk in this block */
};
/* related methods */
dcParserChunk * dc_parser_chunk_new(
  const char *text
);
void dc_parser_chunk_free(
  dcParserChunk **ptr
);

/**
 * A block of control information (field with parameters)
 *
 * Each dcParserBlock contains one or more dcParserChunk objects, which each
 * hold a blob of data.
 */
struct _dcParserBlock
{
  char *name; /**< The block name (e.g., Description) */

  dcParserChunk *head; /**< First chunk in this block */
  dcParserChunk *tail; /**< Last chunk in this block */

  dcParserBlock *next; /**< Next block in section */
  dcParserBlock *prev; /**< Previous block in section */
};
/* related methods */
dcParserBlock * dc_parser_block_new(
  const char *name
);
void dc_parser_block_append(
  dcParserBlock *block,
  dcParserChunk *chunk
);
void dc_parser_block_prepend(
  dcParserBlock *block,
  dcParserChunk *chunk
);
void dc_parser_block_delete(
  dcParserBlock *block,
  dcParserChunk **chunk
);
dcString * dc_parser_block_string(
  dcParserBlock *block
);
void dc_parser_block_free(
  dcParserBlock **ptr
);

/**
 * A section of control information (source, binary, etc)
 *
 * Each dcParserSection contains one or more dcParserBlock objects, which
 * each represent a given control paragraph.
 */
struct _dcParserSection
{
  dcParserBlock *head; /**< First block in this section */
  dcParserBlock *tail; /**< Last block in this section */

  dcParserSection *next; /**< Next section */
};
/* related methods */
dcParserSection * dc_parser_section_new(
  void
);
void dc_parser_section_append(
  dcParserSection *section,
  dcParserBlock *block
);
dcParserBlock * dc_parser_section_find(
  dcParserSection *section,
  const char *field
);
void dc_parser_section_free(
  dcParserSection **ptr
);

/**
 * A Parser state object
 *
 * Each dcParser represents the complete state of a given Debian Control
 * metadata file.
 */
struct _dcParser
{
  dcParserContext ctx; /**< Tracks current parsing context */
  dcErrorHandler handler; /**< Warning/error handler */

  dcParserSection *head; /**< First dcParserSection in this file */
  dcParserSection *tail; /**< Last dcParserSection in this file */
};
/* related methods */
dcParser * dc_parser_new(
  void
);
dcStatus dc_parser_read_file(
  dcParser *parser,
  const char *path
);
dcStatus dc_parser_read_line(
  dcParser *parser,
  char *line,
  size_t len
);
void dc_parser_append(
  dcParser *parser,
  dcParserSection *section
);
void dc_parser_free(
  dcParser **ptr
);

#endif /* DEBCTRL_PARSER_H */
