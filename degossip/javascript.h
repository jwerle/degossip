
#ifndef DEGOSSIP_JAVASCRIPT_H
#define DEGOSSIP_JAVASCRIPT_H

#include <v8.h>
#include "degossip.h"

int
dg_script (dg_t *, const char *);

void
dg_script_report_exception (v8::Isolate *, v8::TryCatch *);

#endif
