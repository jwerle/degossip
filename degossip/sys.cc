
#include <stdio.h>
#include <stdlib.h>
#include <v8.h>

#include "sys.h"
#include "v8macro.h"

void
dg_v8_sys_system (const v8::FunctionCallbackInfo<v8::Value> &arguments) {
  // scope
  V8SCOPE(arguments);

  // ensure one argument
  if (1 != arguments.Length()) {
    V8THROW("Expecting 1 argument.");
    V8RETURN(arguments, V8NULL());
  }

  // command
  v8::String::Utf8Value cmd(arguments[0]);

  // execute and return status code
  V8RETURN(arguments, V8NUMBER(system(*cmd)));
}
