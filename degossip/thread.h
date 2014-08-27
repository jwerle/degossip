
#ifndef DG_THREAD_H
#define DG_THREAD_H

#include <v8.h>
#include <pthread.h>

typedef struct {
  pthread_mutex_t lock;
  pthread_cond_t signal;
  pthread_t thread;
  v8::Isolate *isolate;
  //v8::Handle<v8::Function> *fn;
  v8::Persistent<
    v8::Function,
    v8::CopyablePersistentTraits<v8::Function>> *fn;
} dg_v8_thread_context_t;

void
dg_v8_thread_create_context (const v8::FunctionCallbackInfo<v8::Value> &);

void
dg_v8_thread_run (const v8::FunctionCallbackInfo<v8::Value> &);

#endif
