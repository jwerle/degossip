
#include <stdio.h>
#include <stdlib.h>

#include "degossip.h"
#include "javascript.h"

dg_t *
dg_alloc (int argc, char **argv, char **env) {
  // alloc
  dg_t *self = (dg_t *) malloc(sizeof(dg_t));

  if (NULL == self) {
    return NULL;
  }

  // init
  self->isolate = v8::Isolate::New();
  self->argc = argc;
  self->argv = argv;
  self->env = env;

  // lock isolate
  v8::Locker lock(self->isolate);

  // scope
  v8::Isolate::Scope isolate_scope(self->isolate);
  v8::HandleScope handle_scope(self->isolate);

  // global
  v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(
      self->isolate);

  // context
  v8::Handle<v8::Context> context = v8::Context::New(
      self->isolate, NULL, global);
  v8::Context::Scope context_scope(context);

  // runtime
  context->Enter();
  int rc = dg_script(self, DG_JS_PATH);
  context->Exit();

  if (0 != rc) {
    free(self);
    return NULL;
  }

  return self;
}
