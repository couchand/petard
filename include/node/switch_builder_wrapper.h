// switch builder wrapper

#ifndef SWITCH_BUILDER_WRAPPER_H
#define SWITCH_BUILDER_WRAPPER_H

#include <nan.h>
using Nan::ObjectWrap;

using namespace v8;

#include "switch_builder.h"

class SwitchBuilderWrapper : public Nan::ObjectWrap
{
    SwitchBuilderWrapper(SwitchBuilder *s)
    : Switch (s) {}

    static NAN_METHOD(New);
    static NAN_METHOD(AddCase);

public:
    static Handle<Value> wrapSwitchBuilder(SwitchBuilder *switchBuilder);

    SwitchBuilder *Switch;

    static Nan::Persistent<FunctionTemplate> prototype;

    static inline Nan::Persistent<Function>& constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }

    static NAN_MODULE_INIT(Init);
};

#endif
