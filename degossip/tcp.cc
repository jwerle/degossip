
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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

  if (2 != arguments.Length()) {
    V8THROW("Expecting 2 arguments.");
    v8::Unlocker unlock(isolate);
    return;
  }

  // ensure instance
  if (!arguments.IsConstructCall()) {
    v8::Handle<v8::Value> args[2];
    args[0] = arguments[0];
    args[1] = arguments[1];
    V8RETURN(arguments, arguments.Callee()->NewInstance(2, args));
    return;
  } else {
    self = arguments.This();
  }

  // unwrap
  ctx = (dg_v8_tcp_context_t *) V8UNWRAP(arguments[0]->ToObject());

  // socket type
  int type = arguments[1]->ToNumber()->Int32Value();

  // create socket
  void *socket = zmq_socket(ctx->zctx, type);

  // wrap
  self->SetInternalField(0, v8::External::New(isolate, socket));

  // unlock isolate
  v8::Unlocker unlock(isolate);

  V8RETURN(arguments, self);
}

void
dg_v8_tcp_socket_bind (const v8::FunctionCallbackInfo<v8::Value> &arguments) {
  // isolate
  v8::Isolate *isolate = arguments.GetIsolate();

  // lock
  v8::Locker lock(isolate);

  // scope
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);

  // `this'
  v8::Handle<v8::Object> self = arguments.This();

  if (1 != arguments.Length()) {
    V8THROW("Expecting 1 arguments.");
    v8::Unlocker unlock(isolate);
    return;
  }

  // unwrap
  void *socket = (void *) V8UNWRAP(self);

  // addr
  v8::String::Utf8Value addr(arguments[0]);

  if (0 != zmq_bind(socket, (const char *) *addr)) {
    V8THROW(strerror(errno));
  }

  // unlock isolate
  v8::Unlocker unlock(isolate);

  V8RETURN(arguments, self);
}

void
dg_v8_tcp_socket_recv (const v8::FunctionCallbackInfo<v8::Value> &arguments) {
  // isolate
  v8::Isolate *isolate = arguments.GetIsolate();

  // lock
  v8::Locker lock(isolate);

  // scope
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);

  // `this'
  v8::Handle<v8::Object> self = arguments.This();

  if (1 != arguments.Length()) {
    V8THROW("Expecting 1 argument.");
    v8::Unlocker unlock(isolate);
    return;
  }

  // unwrap
  void *socket = (void *) V8UNWRAP(self);

  // size
  size_t size = (size_t) arguments[0]->ToNumber()->Int32Value();

  // buf
  char buf[size];

  // return value
  v8::Handle<v8::Value> ret;

  // read
  size_t nread = zmq_recv(socket, buf, size, 0);
  if (-1 == nread) {
    V8THROW(strerror(errno));
  }

  buf[nread] = '\0';

  if (nread > 0) {
    ret = V8STRING(buf);
  } else {
    ret = V8NULL();
  }

  //printf("%s\n", buf);
  //printf("%zu %zu %s\n", size, nread, buf);

  // unlock isolate
  v8::Unlocker unlock(isolate);

  V8RETURN(arguments, ret);
}
