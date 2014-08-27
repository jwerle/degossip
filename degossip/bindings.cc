
#include <stdio.h>
#include <stdlib.h>
#include <v8.h>
#include "degossip.h"
#include "bindings.h"
#include "v8macro.h"

#include "thread.h"
#include "io.h"

void
dg_v8_bindings_init (dg_t *dg) {
  // thread
  {
    v8::Local<v8::FunctionTemplate> fn = V8FUNCTION(dg_v8_thread_create_context);
    v8::Local<v8::ObjectTemplate> proto = fn->PrototypeTemplate();

    // class
    fn->SetHiddenPrototype(true);
    fn->SetClassName(V8STRING("Thread"));
    fn->InstanceTemplate()->SetInternalFieldCount(1);

    // prototype
    proto->Set(V8STRING("run"), V8FUNCTION(dg_v8_thread_run));
    DG_V8_SET_BINDING(dg, "Thread", fn);
  }

  // io
  {
    DG_V8_SET_BINDING(dg, "stdin", V8NUMBER(0));
    DG_V8_SET_BINDING(dg, "stdout", V8NUMBER(1));
    DG_V8_SET_BINDING(dg, "stderr", V8NUMBER(2));
    DG_V8_SET_BINDING(dg, "read", V8FUNCTION(dg_v8_io_read));
    DG_V8_SET_BINDING(dg, "write", V8FUNCTION(dg_v8_io_write));
  }
}
