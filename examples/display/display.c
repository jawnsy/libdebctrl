#include <debctrl.h>
#include <stdio.h>
#include "debug.h"

void usage(void);

int main(int argc, char *argv[])
{
  dcParser *parser;
  char *path;

  if (argc > 1 && argv[1] != NULL)
    path = argv[1];
  else
    path = "debian/control";

  parser = dc_parser_new();
  dc_parser_read_file(parser, path);
  dc_parser_dump(parser);
  dc_parser_free(&parser);

  return 0;
}

void usage(void)
{
  printf(
    "Usage:\n"
    "  display -?\n"
    "  display [-d] [CONTROL FILE]\n"
    "Notes:\n"
    "  - Using the -d flag turns on debug mode, printing some additional\n"
    "    in-flight information that probably isn't too useful.\n"
    "  - If no file is specified, this looks for 'debian/control'\n"
    "\n"
  );
}
