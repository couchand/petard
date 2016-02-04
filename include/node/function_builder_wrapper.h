// function builder

#ifndef FUNCTION_BUILDER_WRAPPER_H
#define FUNCTION_BUILDER_WRAPPER_H

#include <nan.h>
using Nan::ObjectWrap;

using namespace v8;

#include "function_builder.h"

#include "builder_wrapper.h"

class FunctionBuilderWrapper : public BuilderWrapper
{
    explicit FunctionBuilderWrapper(FunctionBuilder *builder)
    : BuilderWrapper(builder) {}

    static NAN_METHOD(New);
    static NAN_GETTER(GetName);
    static NAN_GETTER(GetType);

public:
    static Nan::Persistent<FunctionTemplate> prototype;

    static inline Nan::Persistent<Function>& constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }

    static NAN_MODULE_INIT(Init);

    FunctionBuilder *getFunctionBuilder()
    {
        return static_cast<FunctionBuilder *>(Builder);
    }
};

#endif
