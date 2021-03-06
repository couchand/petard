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
    static NAN_METHOD(FDiv);
    static NAN_METHOD(URem);
    static NAN_METHOD(SRem);
    static NAN_METHOD(FRem);
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
    static NAN_METHOD(FOEqual);
    static NAN_METHOD(FONotEqual);
    static NAN_METHOD(FOGreaterThan);
    static NAN_METHOD(FOAtLeast);
    static NAN_METHOD(FOLessThan);
    static NAN_METHOD(FOAtMost);
    static NAN_METHOD(FUEqual);
    static NAN_METHOD(FUNotEqual);
    static NAN_METHOD(FUGreaterThan);
    static NAN_METHOD(FUAtLeast);
    static NAN_METHOD(FULessThan);
    static NAN_METHOD(FUAtMost);

    static NAN_METHOD(Trunc);
    static NAN_METHOD(ZExt);
    static NAN_METHOD(SExt);
    static NAN_METHOD(FPToUI);
    static NAN_METHOD(FPToSI);
    static NAN_METHOD(UIToFP);
    static NAN_METHOD(SIToFP);
    static NAN_METHOD(FPTrunc);
    static NAN_METHOD(FPExt);
    static NAN_METHOD(PtrToInt);
    static NAN_METHOD(IntToPtr);
    static NAN_METHOD(Bitcast);

    static NAN_METHOD(Select);

    static NAN_METHOD(Value);
    static NAN_METHOD(Undefined);

    static NAN_METHOD(Return);
    static NAN_METHOD(Parameter);

    static NAN_METHOD(LoadConstant);
    static NAN_METHOD(GetElementPointer);
    static NAN_METHOD(ExtractElement);
    static NAN_METHOD(InsertElement);

    static NAN_METHOD(CallFunction);

    static NAN_METHOD(Br);
    static NAN_METHOD(CondBr);
    static NAN_METHOD(Switch);

    static NAN_METHOD(CreateBlock);
    static NAN_METHOD(SplitBlock);
    static NAN_METHOD(UseBlock);
    static NAN_METHOD(InsertBefore);
    static NAN_METHOD(InsertAfter);

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
