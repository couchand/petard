// value wrapper

#ifndef VALUE_WRAPPER_H
#define VALUE_WRAPPER_H

#include <nan.h>
using Nan::ObjectWrap;

using namespace v8;

#include "value.h"

class ValueWrapper : public Nan::ObjectWrap
{
    ValueWrapper(ValueHandle *v)
    : Val(v) {}

    static NAN_METHOD(New);
    static NAN_GETTER(GetType);

public:
    static Handle<Value> wrapValue(ValueHandle *value);

    ValueHandle *Val;

    static Nan::Persistent<FunctionTemplate> prototype;

    static inline Nan::Persistent<Function>& constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }

    static NAN_MODULE_INIT(Init);
};

#endif
