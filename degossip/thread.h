
#ifndef DG_THREAD_H
#define DG_THREAD_H

#include <v8.h>
#include <pthread.h>
#include <vector>

typedef struct dg_v8_thread_context_t dg_v8_thread_context_t;
struct dg_v8_thread_context_t {
  pthread_mutex_t lock;
  pthread_cond_t signal;
  pthread_t thread;
  v8::Isolate *isolate;

  std::vector<
    v8::Persistent<
      v8::Function,
      v8::CopyablePersistentTraits<v8::Function>>> fn;

  v8::Persistent<
    v8::Value,
    v8::CopyablePersistentTraits<v8::Value>> data;

  v8::Persistent<
    v8::Object,
    v8::CopyablePersistentTraits<v8::Object>> ref;

};

void
dg_v8_thread_create_context (const v8::FunctionCallbackInfo<v8::Value> &);

void
dg_v8_thread_run (const v8::FunctionCallbackInfo<v8::Value> &);

void
dg_v8_thread_wait (const v8::FunctionCallbackInfo<v8::Value> &);

void
dg_v8_thread_exit (const v8::FunctionCallbackInfo<v8::Value> &);

#endif
