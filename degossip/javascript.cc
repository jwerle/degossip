
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <v8.h>

#include "degossip.h"
#include "javascript.h"

#define cstr(v) (*v? *v : "")

int
dg_script (dg_t *dg, const char *path) {
  // file
  FILE *file = fopen(path, "r");

  // silent error...
  if (NULL == file) {
    printf("womp %s\n", path);
    return 1;
  }

  // source size
  size_t size = 0; {
    unsigned long pos = ftell(file);
    rewind(file);
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, pos, SEEK_SET);
  }

  // source
  char *src = (char *) malloc(sizeof(char) * (size + 1));
  fread(src, 1, size, file);
  src[size] = '\0';
  fclose(file);
  file = NULL;

  // isolate
  v8::Isolate *isolate = dg->isolate;

  // thread isolate lock
  v8::Locker lock(isolate);

  // scope
  v8::Isolate::Scope isolate_scope(isolate);
  v8::EscapableHandleScope handle_scope(isolate);

  // v8ify source and basename
  v8::Handle<v8::String> str = v8::String::NewFromUtf8(isolate, src);
  v8::Handle<v8::String> name = v8::String::NewFromUtf8(
      isolate, basename((char *) path));

  // exception handling
  v8::TryCatch tc;

  // compile
  v8::ScriptOrigin origin(name);
  v8::Handle<v8::Script> script = v8::Script::Compile(str, &origin);

  // dealloc
  free(src);
  src = NULL;

  if (script.IsEmpty()) {
    dg_script_report_exception(isolate, &tc);
    return 1;
  }

  // run!
  v8::Local<v8::Value> result = script->Run();

  // face palm
  if (result.IsEmpty()) {
    if (tc.HasCaught()) {
      dg_script_report_exception(isolate, &tc);
      return 1;
    }
  }

  return 0;
}

void
dg_script_report_exception (v8::Isolate *isolate, v8::TryCatch *tc) {
  // scope
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);

  // exception string
  v8::String::Utf8Value exception(tc->Exception());
  v8::Handle<v8::Message> message = tc->Message();
  const char *exception_string = cstr(exception);

  if (message.IsEmpty()) {
    fprintf(stderr, "%s\n", exception_string);
  } else {
    v8::String::Utf8Value filename(message->GetScriptResourceName());
    v8::String::Utf8Value sourceline(message->GetSourceLine());
    v8::String::Utf8Value stack_trace(tc->StackTrace());
    const char *filename_string = cstr(filename);
    int i = 0;
    int linenum = message->GetLineNumber();
    int start = message->GetStartColumn();
    int end = message->GetEndColumn();
    const char *sourceline_string = cstr(sourceline);
    const char *stack_trace_string = NULL;

    fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
    fprintf(stderr, "%s\n", sourceline_string);

    for (i = 0; i < start; i++) { fprintf(stderr, " "); }
    for (i = start; i < end; i++) { fprintf(stderr, "^"); }

    fprintf(stderr, "\n");

    if (stack_trace.length() > 0) {
      stack_trace_string = cstr(stack_trace);
      fprintf(stderr, "%s\n", stack_trace_string);
    }
  }
}

