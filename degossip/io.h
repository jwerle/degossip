
#ifndef DG_IO_H
#define DG_IO_H

#include <v8.h>

void
dg_v8_io_read (const v8::FunctionCallbackInfo<v8::Value> &);

void
dg_v8_io_write (const v8::FunctionCallbackInfo<v8::Value> &);

#endif
