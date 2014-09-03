
#include <stdio.h>
#include <stdlib.h>
#include <v8.h>
#include <zmq.h>

#include "degossip.h"
#include "bindings.h"
#include "v8macro.h"

#include "io.h"
#include "tcp.h"
#include "sys.h"
#include "thread.h"

#define BUFMAX 4096

void
dg_v8_noop (const v8::FunctionCallbackInfo<v8::Value> &arguments) {
  V8SCOPE(arguments);
}

static int
_set_argv (dg_t *);

static int
_set_env (dg_t *);

void
dg_v8_bindings_init (dg_t *dg) {

  // environemnt
  {
    _set_argv(dg);
    _set_env(dg);
  }

  // system
  {
    // functions
    DG_V8_SET_BINDING(dg, "system", V8FUNCTION(dg_v8_sys_system));

    // constants
    DG_V8_SET_BINDING(dg, "BUFSIZ", V8NUMBER(BUFSIZ));
  }

  // io
  {
    DG_V8_SET_BINDING(dg, "stdin", V8NUMBER(0));
    DG_V8_SET_BINDING(dg, "stdout", V8NUMBER(1));
    DG_V8_SET_BINDING(dg, "stderr", V8NUMBER(2));
    DG_V8_SET_BINDING(dg, "read", V8FUNCTION(dg_v8_io_read));
    DG_V8_SET_BINDING(dg, "write", V8FUNCTION(dg_v8_io_write));
  }

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
    proto->Set(V8STRING("wait"), V8FUNCTION(dg_v8_thread_wait));
    proto->Set(V8STRING("exit"), V8FUNCTION(dg_v8_thread_exit));

    // bindings
    DG_V8_SET_BINDING(dg, "Thread", fn);
  }

  // tcp
  {
    // classes
    v8::Local<v8::FunctionTemplate> TCPContext = V8FUNCTION(dg_v8_tcp_create_context);
    v8::Local<v8::FunctionTemplate> TCPSocket = V8FUNCTION(dg_v8_tcp_create_socket);

    // prototypes
    v8::Local<v8::ObjectTemplate> TCPContextPrototype = TCPContext->PrototypeTemplate();
    v8::Local<v8::ObjectTemplate> TCPSocketPrototype = TCPSocket->PrototypeTemplate();

    // TCPContext class
    TCPContext->SetHiddenPrototype(true);
    TCPContext->SetClassName(V8STRING("TCPContext"));
    TCPContext->InstanceTemplate()->SetInternalFieldCount(1);

    // TCPSocket class
    TCPSocket->SetHiddenPrototype(true);
    TCPSocket->SetClassName(V8STRING("TCPSocket"));
    TCPSocket->InstanceTemplate()->SetInternalFieldCount(1);

    // TCPContext prototype
    TCPContextPrototype->Set(V8STRING("destroy"), V8FUNCTION(dg_v8_tcp_destroy_context));

    // TCPSocket prototype
    TCPSocketPrototype->Set(V8STRING("connect"), V8FUNCTION(dg_v8_tcp_socket_connect));
    TCPSocketPrototype->Set(V8STRING("bind"), V8FUNCTION(dg_v8_tcp_socket_bind));
    TCPSocketPrototype->Set(V8STRING("read"), V8FUNCTION(dg_v8_tcp_socket_recv));
    TCPSocketPrototype->Set(V8STRING("write"), V8FUNCTION(dg_v8_tcp_socket_send));
    TCPSocketPrototype->Set(V8STRING("close"), V8FUNCTION(dg_v8_tcp_socket_close));

    // constants
    TCPSocket->Set(V8STRING("REQUEST"), V8NUMBER(ZMQ_REQ));
    TCPSocket->Set(V8STRING("REPLY"), V8NUMBER(ZMQ_REP));
    TCPSocket->Set(V8STRING("DEALER"), V8NUMBER(ZMQ_DEALER));
    TCPSocket->Set(V8STRING("ROUTER"), V8NUMBER(ZMQ_ROUTER));
    TCPSocket->Set(V8STRING("PUBLISH"), V8NUMBER(ZMQ_PUB));
    TCPSocket->Set(V8STRING("SUBSCRIBE"), V8NUMBER(ZMQ_SUB));
    TCPSocket->Set(V8STRING("PUSH"), V8NUMBER(ZMQ_PUSH));
    TCPSocket->Set(V8STRING("PULL"), V8NUMBER(ZMQ_PULL));
    TCPSocket->Set(V8STRING("PAIR"), V8NUMBER(ZMQ_PAIR));
    TCPSocket->Set(V8STRING("STREAM"), V8NUMBER(ZMQ_STREAM));
    TCPSocket->Set(V8STRING("SEND_MORE"), V8NUMBER(ZMQ_SNDMORE));
    TCPSocket->Set(V8STRING("NOWAIT"), V8NUMBER(ZMQ_DONTWAIT));

    // bindings
    DG_V8_SET_BINDING(dg, "TCPContext", TCPContext);
    DG_V8_SET_BINDING(dg, "TCPSocket", TCPSocket);
  }
}

static int
_set_argv (dg_t *dg) {
  int i = 0;
  int argc = 0;
  char str[1024];
  char **argv = dg->argv;
  char *arg = NULL;
  const char *delim = "_$$$_";

  // isolate
  v8::Isolate *isolate = v8::Isolate::GetCurrent();

  // scope
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);

  // build str
  memset(str, 0, 1024);
  while ((arg = argv[i++])) {
    strcat(str, arg);
    strcat(str, delim);
  }

  // set `__ARGV__' string
  DG_V8_SET_BINDING(dg, "ARGV", V8STRING(str));
  return 0;
}

static int
_set_env (dg_t *dg) {
  char **env = dg->env;
  char *line = NULL;
  char key[BUFMAX];
  char value[BUFMAX];
  char tmp[BUFMAX];
  char out[BUFMAX];
  char ch = 0;
  int j = 0;
  int i = 0;
  int body = 0;
  size_t size = 1 ;

  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope hscope(isolate);
  v8::Handle<v8::ObjectTemplate> ENV = v8::ObjectTemplate::New(isolate);

  // init
  memset(out, 0, BUFMAX);

  // parse
  while ((line = env[j++])) {
    i = 0;
    size = 0;

    memset(tmp, 0, BUFMAX);
    memset(key, 0, BUFMAX);
    memset(value, 0, BUFMAX);

    do {
      ch = line[i];
      if ('=' == ch) {
        // set key
        strncat(key, tmp, size);
        key[size] = '\0';
        size = 0;
      } else {
        tmp[size++] = ch;
      }
    } while ('\0' != (ch = line[++i]));

    // set value
    strncat(value, tmp, size);
    value[size] = '\0';

    ENV->Set(V8STRING(key), V8STRING(value));
  }

  DG_V8_SET_BINDING(dg, "ENV", ENV);
  return 0;
}
