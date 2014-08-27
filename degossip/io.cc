
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <v8.h>
#include "v8macro.h"
#include "io.h"

void
dg_v8_io_read (const v8::FunctionCallbackInfo<v8::Value> &arguments) {
  char buf[BUFSIZ];
  int fd = 0;
  size_t size = 0;
  ssize_t bytes = 0;

  // scope
  V8SCOPE(arguments);

  if (2 != arguments.Length()) {
    V8RETURN(arguments, V8NULL());
  }

  // fd
  fd = arguments[0]->ToNumber()->Int32Value();

  // size
  size = arguments[1]->ToNumber()->Int32Value();

  // read
  bytes = read(fd, buf, size);
  buf[bytes] = '\0';

  if (0 == bytes) {
    V8RETURN(arguments, V8NULL());
  } else {
    V8RETURN(arguments, V8STRING(buf));
  }
}

void
dg_v8_io_write (const v8::FunctionCallbackInfo<v8::Value> &arguments) {
  int fd = 0;
  size_t size = 0;
  ssize_t bytes = 0;

  // scope
  V8SCOPE(arguments);

  if (3 != arguments.Length()) {
    V8RETURN(arguments, V8NULL());
  }

  // fd
  fd = arguments[0]->ToNumber()->Int32Value();

  // buf
  v8::String::Utf8Value buf(arguments[1]);

  // size
  size = arguments[2]->ToNumber()->Int32Value();

  // write
  bytes = write(fd, *buf, size);

  if (0 == bytes) {
    V8RETURN(arguments, V8FALSE());
  } else {
    V8RETURN(arguments, V8TRUE());
  }
}
