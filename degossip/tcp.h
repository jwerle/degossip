
#ifndef DG_TCP_H
#define DG_TCP_H

#include <v8.h>
#include <zmq.h>

typedef struct dg_v8_tcp_context_t dg_v8_tcp_context_t;
struct dg_v8_tcp_context_t {
  void *zctx;
};

void
dg_v8_tcp_create_context (const v8::FunctionCallbackInfo<v8::Value> &);

void
dg_v8_tcp_destroy_context (const v8::FunctionCallbackInfo<v8::Value> &);

void
dg_v8_tcp_create_socket (const v8::FunctionCallbackInfo<v8::Value> &);

void
dg_v8_tcp_socket_bind (const v8::FunctionCallbackInfo<v8::Value> &);

void
dg_v8_tcp_socket_recv (const v8::FunctionCallbackInfo<v8::Value> &);

void
dg_v8_tcp_socket_send (const v8::FunctionCallbackInfo<v8::Value> &);

void
dg_v8_tcp_socket_close (const v8::FunctionCallbackInfo<v8::Value> &);

#endif
