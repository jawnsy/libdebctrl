#include <debctrl.h>
#include <stdio.h>
#include "debug.h"

void dc_parser_dump(dcParser *parser)
{
  dcParserSection *cur = parser->head;
  int i = 1;

  while (cur != NULL)
  {
    printf("------ Section %d ------\n", i++);
    dc_parser_section_dump(cur);
    cur = cur->next;
  }
}

void dc_parser_section_dump(dcParserSection *section)
{
  dcParserBlock *cur = section->head;

  while (cur != NULL)
  {
    printf("  %s\n", cur->name);
    dc_parser_block_dump(cur);

    cur = cur->next;
  }
}

void dc_parser_block_dump(dcParserBlock *block)
{
  dcParserChunk *cur = block->head;

  while (cur != NULL)
  {
    if (cur->type == CHUNK_FIXED)
      printf("[fixed] %s\n", cur->text);
    else if (cur->type == CHUNK_MERGE)
      printf("[merge] %s\n", cur->text);
    else
      printf("[empty]\n");

    cur = cur->next;
  }
}
