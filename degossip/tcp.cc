
#include <stdio.h>
#include <stdlib.h>
#include <zmq.h>

#include "v8macro.h"
#include "tcp.h"

void
dg_v8_tcp_create_context (const v8::FunctionCallbackInfo<v8::Value> &arguments) {
  dg_v8_tcp_context_t *ctx = NULL;

  // isolate
  v8::Isolate *isolate = arguments.GetIsolate();

  // lock
  v8::Locker lock(isolate);

  // scope
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);

  // `this'
  v8::Handle<v8::Object> self;

  // ensure instance
  if (!arguments.IsConstructCall()) {
    V8RETURN(arguments, arguments.Callee()->NewInstance(0, NULL));
    return;
  } else {
    self = arguments.This();
  }

  // alloc
  ctx = new dg_v8_tcp_context_t();

  if (NULL == ctx) {
    V8THROW("Failed to create tcp context.");
    return;
  }

  // init
  ctx->zctx = zmq_ctx_new();

  // wrap
  self->SetInternalField(0, v8::External::New(isolate, ctx));

  // unlock isolate
  v8::Unlocker unlock(isolate);

  V8RETURN(arguments, self);
}

void
dg_v8_tcp_create_socket (const v8::FunctionCallbackInfo<v8::Value> &arguments) {
  dg_v8_tcp_context_t *ctx = NULL;

  // isolate
  v8::Isolate *isolate = arguments.GetIsolate();

  // lock
  v8::Locker lock(isolate);

  // scope
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);

  // `this'
  v8::Handle<v8::Object> self;

  if (arguments.IsConstructCall()) {
    V8THROW("Invalid `this' receiver.");
    return;
  }

  // unwrap
  ctx = (dg_v8_tcp_context_t *) V8UNWRAP(self);

}
