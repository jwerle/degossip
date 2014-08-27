
#ifndef DG_V8_MACRO
#define DG_V8_MACRO

#include <v8.h>

#define V8H(f, v) f(v8::Isolate::GetCurrent(), v)
#define V8STRING(v) V8H(v8::String::NewFromUtf8, v)
#define V8FUNCTION(v) V8H(v8::FunctionTemplate::New, v)
#define V8NUMBER(v) V8H(v8::Number::New, v)
#define V8NULL() v8::Null(v8::Isolate::GetCurrent())
#define V8TRUE() v8::True(v8::Isolate::GetCurrent())
#define V8FALSE() v8::False(v8::Isolate::GetCurrent())
#define V8RETURN(a, v) return a.GetReturnValue().Set(v);
#define V8THROW(err) v8::Isolate::GetCurrent()->ThrowException(V8STRING(err))
#define V8EXTERNAL(v) V8H(v8::External::New, v)
#define V8DATE(t) V8H(v8::Date::New, t)

#define V8OBJECT()                                                             \
  v8::ObjectTemplate::New(v8::Isolate::GetCurrent())->NewInstance()

#define DG_V8_SET_BINDING(dg, k, v) dg->global->Set(V8STRING(k), v)
#define DG_V8_GET_BINDING(dg, k) dg->global->Get(V8STRING(k))

#define V8SCOPE(args) ({                                                       \
  v8::Isolate *iso = args.GetIsolate();                                        \
  v8::Isolate::Scope isolate_scope(iso);                                       \
  v8::HandleScope scope(iso);                                                  \
})


#define V8WRAP(ptr) ({                                                         \
  v8::Handle<v8::ObjectTemplate> tpl =                                         \
  v8::ObjectTemplate::New(v8::Isolate::GetCurrent());                          \
  tpl->SetInternalFieldCount(1);                                               \
  v8::Local<v8::Object> obj = tpl->NewInstance();                              \
  obj->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), ptr)); \
  obj;                                                                         \
})

#define V8UNWRAP(ref) ({                                                       \
  v8::Local<v8::External> wrap =                                               \
  v8::Local<v8::External>::Cast(ref->GetInternalField(0));                     \
  wrap->Value();                                                               \
})

#endif
