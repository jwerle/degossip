
#ifndef DEGOSSIP_H
#define DEGOSSIP_H

#include <v8.h>

#ifndef DG_JS_PATH
#error "`DG_JS_PATH' undefined"
#endif

typedef struct {
  v8::Isolate *isolate;
  int argc;
  char **argv;
  char **env;
} dg_t;

dg_t *
dg_alloc (int, char **, char **);

#endif
