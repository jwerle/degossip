
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <v8.h>
#include <pthread.h>
#include "v8macro.h"
#include "thread.h"
#include "javascript.h"

#include <time.h>

static void *
onthread (void *data) {
  dg_v8_thread_context_t *ctx = NULL;

  // thread context
  ctx = (dg_v8_thread_context_t *) data;

  // isolate
  v8::Isolate *isolate = ctx->isolate;
  if (NULL == isolate) {
    isolate = v8::Isolate::New();
  }

  // locker
  v8::Locker lock(isolate);

  // scope
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);

  v8::Persistent<
    v8::Function,
    v8::CopyablePersistentTraits<v8::Function>> ref = ctx->fn.back();
  v8::Local<v8::Function> cb = v8::Local<v8::Function>::New(isolate, ref);

  v8::TryCatch tc;

  v8::Handle<v8::Value> args[1];
  v8::Local<v8::Value> arg = v8::Local<v8::Value>::New(isolate, ctx->data);
  args[0] = arg;

  struct timespec req {.tv_sec = 0, .tv_nsec = 2000};
  struct timespec rem;

  cb->Call(cb, 1, args);

  if (tc.HasCaught()) {
     //@TODO - handle exception
  }

  nanosleep(&req, &rem);
  v8::Unlocker unlock(isolate);

  pthread_exit(NULL);

  return NULL;
}

void
dg_v8_thread_create_context (const v8::FunctionCallbackInfo<v8::Value> &arguments) {
  dg_v8_thread_context_t *ctx = NULL;

  // isolate
  v8::Isolate *isolate = arguments.GetIsolate();

  // lock
  v8::Locker lock(isolate);

  // scope
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);

  // `this'
  v8::Handle<v8::Object> self;

  if (1 != arguments.Length() || !arguments[0]->ToObject()->IsCallable()) {
    V8THROW("Invalid arguments");
    return;
  }

  // get `this'
  if (!arguments.IsConstructCall()) {
    v8::Handle<v8::Value> args[1];
    args[0] = arguments[0];
    V8RETURN(arguments, arguments.Callee()->NewInstance(1, args));
    return;
  } else {
    self = arguments.This();
  }

  // alloc
  ctx = new dg_v8_thread_context_t();

  if (NULL == ctx) {
    V8THROW("Failed to create thread context.");
    return;
  }

  // callback
  v8::Handle<v8::Function> cb = v8::Handle<v8::Function>::Cast(arguments[0]);

  // persistent callback wrap
  v8::Persistent<v8::Function,
    v8::CopyablePersistentTraits<v8::Function>> ref(isolate, cb);

  // attach
  ctx->fn.push_back(ref);
  ctx->isolate = isolate;

  // wrap
  self->SetInternalField(0, v8::External::New(isolate, ctx));

  // unlock
  v8::Unlocker unlock(isolate);

  V8RETURN(arguments, self);
}

void
dg_v8_thread_run (const v8::FunctionCallbackInfo<v8::Value> &arguments) {
  dg_v8_thread_context_t *ctx = NULL;

  // `this'
  v8::Handle<v8::Object> self;

  // isolate
  v8::Isolate *isolate = arguments.GetIsolate();
  v8::Locker lock(isolate);

  // scope
  V8SCOPE(arguments);

  if (arguments.IsConstructCall()) {
    V8THROW("Invalid `this' receiver.");
    return;
  }

  // `this'
  self = arguments.This();

  // arg
  ctx = (dg_v8_thread_context_t *) V8UNWRAP(self);

  v8::Persistent<v8::Value,
    v8::CopyablePersistentTraits<v8::Value>> data(
        isolate, arguments[0]);

  ctx->data = data;

  if (0 != pthread_create(&ctx->thread, NULL, &onthread, ctx)) {
    V8THROW("Error creating execution thread in context.");
    return;
  }

  // unlock isolate
  v8::Unlocker unlock(isolate);

  if (0 != pthread_join(ctx->thread, NULL)) {
    V8THROW("Error joining thread in context.");
    return;
  }
}

