// builder

#ifndef BUILDER_WRAPPER_H
#define BUILDER_WRAPPER_H

#include <nan.h>
using Nan::ObjectWrap;

using namespace v8;

#include "builder.h"

class BuilderWrapper : public Nan::ObjectWrap
{
protected:
    explicit BuilderWrapper(InstructionBuilder *builder)
    : Builder(builder) {}

    static NAN_METHOD(New);
    static NAN_METHOD(Alloca);
    static NAN_METHOD(Load);
    static NAN_METHOD(Store);

    static NAN_METHOD(Add);
    static NAN_METHOD(Sub);
    static NAN_METHOD(Mul);
    static NAN_METHOD(UDiv);
    static NAN_METHOD(SDiv);
    static NAN_METHOD(URem);
    static NAN_METHOD(SRem);
    static NAN_METHOD(And);
    static NAN_METHOD(Or);
    static NAN_METHOD(Xor);
    static NAN_METHOD(Shl);
    static NAN_METHOD(LShr);
    static NAN_METHOD(AShr);

    static NAN_METHOD(Equal);
    static NAN_METHOD(NotEqual);
    static NAN_METHOD(UGreaterThan);
    static NAN_METHOD(UAtLeast);
    static NAN_METHOD(ULessThan);
    static NAN_METHOD(UAtMost);
    static NAN_METHOD(SGreaterThan);
    static NAN_METHOD(SAtLeast);
    static NAN_METHOD(SLessThan);
    static NAN_METHOD(SAtMost);

    static NAN_METHOD(Select);

    static NAN_METHOD(Value);

    static NAN_METHOD(Return);
    static NAN_METHOD(Parameter);

    static NAN_METHOD(LoadConstant);

    static NAN_METHOD(CallFunction);

    static NAN_METHOD(Br);
    static NAN_METHOD(CondBr);

    static NAN_METHOD(CreateBlock);
    static NAN_METHOD(SplitBlock);

    static NAN_METHOD(If);

public:
    static Handle<v8::Value> wrapBuilder(InstructionBuilder *value);

    InstructionBuilder *Builder;

    static Nan::Persistent<FunctionTemplate> prototype;

    static inline Nan::Persistent<Function>& constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }

    static NAN_MODULE_INIT(Init);
};

#endif
