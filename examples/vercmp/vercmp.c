#include <debctrl.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  char *vstring;
  dcVersion *vobj;

  if (argc > 1 && argv[1] != NULL)
    vstring = argv[1];
  else
  {
    printf("Usage: vercmp <version string>\n");
    return 0;
  }

  vobj = dc_version_new();
  dc_version_set(vobj, vstring);

  printf("Epoch:            %lu\n", vobj->epoch);
  printf("Upstream version: %s\n", vobj->version);
  printf("Debian revision:  %s\n", vobj->revision);

  dc_version_free(&vobj);

  return 0;
}
