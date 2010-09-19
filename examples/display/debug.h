#ifndef DEBCTRL_DEBUG

#include <debctrl.h>

void dc_parser_dump(dcParser *parser);
void dc_parser_section_dump(dcParserSection *section);
void dc_parser_block_dump(dcParserBlock *block);

#define DEBCTRL_DEBUG
#endif /* DEBCTRL_DEBUG */
