
#ifndef DG_BINDINGS_H
#define DG_BINDINGS_H

#include "degossip.h"

void
dg_v8_bindings_init (dg_t *);

void
dg_v8_noop (const v8::FunctionCallbackInfo<v8::Value> &);

#endif
