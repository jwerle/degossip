
#include <stdio.h>
#include <stdlib.h>
#include <v8.h>
#include "degossip.h"

static dg_t *dg = NULL;

int
main (int argc, char **argv, char **env) {
  // init v8
  {
    int i = 0;
    int v8argc = argc;
    char *tmp = NULL;
    char *v8argv[v8argc];
    while ((tmp = argv[i++])) { v8argv[i] = tmp; }
    v8argv[v8argc++] = (char *) "--harmony";
    v8::V8::InitializeICU();
    v8::V8::SetFlagsFromCommandLine(&v8argc, v8argv, true);
  }

  dg = dg_alloc(argc, argv, env);

  if (NULL == dg) {
    return 1;
  }

  return 0;
}
