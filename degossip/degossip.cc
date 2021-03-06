
#include <stdio.h>
#include <stdlib.h>

#include "degossip.h"
#include "bindings.h"
#include "javascript.h"

#ifndef DG_JS_PATH
#define DG_JS_PATH ""
#endif

class MallocArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
  public:
  virtual void *
  Allocate (size_t size) {
    return malloc(size);
  }

  virtual void *
  AllocateUninitialized (size_t size) {
    return malloc(size);
  }

  virtual void
  Free (void *data, size_t size) {
    free(data);
  }
};

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

  v8::V8::SetArrayBufferAllocator(new MallocArrayBufferAllocator());

  // lock isolate
  v8::Locker lock(self->isolate);

  // scope
  v8::Isolate::Scope isolate_scope(self->isolate);
  v8::HandleScope handle_scope(self->isolate);

  // global
  v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(
      self->isolate);
  self->global = global;

  // bindings
  dg_v8_bindings_init(self);

  // context
  v8::Handle<v8::Context> context = v8::Context::New(
      self->isolate, NULL, global);
  v8::Persistent<v8::Context> persistent_context(self->isolate, context);
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
