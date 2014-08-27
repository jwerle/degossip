
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <v8.h>
#include <pthread.h>
#include "v8macro.h"
#include "thread.h"

static void *
onthread (void *data) {
  dg_v8_thread_context_t *ctx = NULL;

  // thread context
  ctx = (dg_v8_thread_context_t *) data;

  // isolate
  v8::Isolate *isolate = ctx->isolate;
  isolate = v8::Isolate::New();

  // locker
  v8::Locker lock(isolate);

  // scope
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);
  v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
  v8::Handle<v8::Context> context = v8::Context::New(isolate, NULL, global);
  v8::Context::Scope context_scope(context);

  v8::Handle<v8::Function> fn = *reinterpret_cast<v8::Handle<v8::Function> *>(&ctx->fn);
  //ctx->fn->Reset(isolate, fn);
  //v8::Handle<v8::Function> fn = *ctx->fn;
  //fn->Call(V8NULL(), 0, NULL);
  printf("biz\n");

  v8::Unlocker unlock(isolate);

  return NULL;
}

void
dg_v8_thread_create_context (const v8::FunctionCallbackInfo<v8::Value> &arguments) {
  dg_v8_thread_context_t *ctx = NULL;

  // `this'
  v8::Handle<v8::Object> self;

  // isolate
  v8::Isolate *isolate = arguments.GetIsolate();

  // scope
  v8::Isolate::Scope isolate_scope(isolate);
  v8::EscapableHandleScope handle_cope(isolate);

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
  ctx = (dg_v8_thread_context_t *) malloc(
      sizeof(dg_v8_thread_context_t) +
      sizeof(pthread_t));

  if (NULL == ctx) {
    V8THROW("Failed to create thread context.");
    return;
  }

  // init lock
  if (0 != pthread_mutex_init(&ctx->lock, NULL)) {
    V8THROW("Failed to lock context mutex.");
    return;
  }

  // init signal
  if (0 != pthread_cond_init(&ctx->signal, NULL)) {
    V8THROW("Failed to lock context signal.");
    return;
  }

  // callback
  v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(arguments[0]);
  v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function> > fn(
      isolate, cb);
  //fn.Reset(isolate, cb);

  // attach
  ctx->fn = &fn;
  ctx->isolate = isolate;

  // wrap
  self->SetInternalField(0, v8::External::New(isolate, ctx));

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

  if (0 != pthread_create(&ctx->thread, NULL, &onthread, ctx)) {
    V8THROW("Error creating execution thread in context.");
    return;
  }

  /*
  // block until unlocked
  if (0 != pthread_mutex_lock(&ctx->lock)) {
    V8THROW("Failed to handle thread lock in context.");
    return;
  }

  // unblock with broadcast signal
  if (0 != pthread_cond_broadcast(&ctx->signal)) {
    V8THROW("Failed to unlock with broadcast signal in context.");
    return;
  }

  // release and unlock
  if (0 != pthread_mutex_unlock(&ctx->lock)) {
    V8THROW("Failed to release and unlock in context.");
    return;
  }

  */
  // join thread
  if (0 != pthread_join(ctx->thread, NULL)) {
    V8THROW("Failed to join thread in context.");
    return;
  }
}

