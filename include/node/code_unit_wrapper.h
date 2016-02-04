// codeunit

#ifndef CODE_UNIT_WRAPPER_H
#define CODE_UNIT_WRAPPER_H

#include <nan.h>
using Nan::ObjectWrap;

using namespace v8;

#include "types.h"
#include "function_builder.h"
#include "codegen.h"

#include "value_wrapper.h"
#include "function_builder_wrapper.h"

class CodeUnitWrapper : public Nan::ObjectWrap
{
    explicit CodeUnitWrapper(CodeUnit *unit)
    : Unit(unit) {}

    static NAN_METHOD(New);

    static NAN_METHOD(Dump);
    static NAN_METHOD(WriteToFile);
    static NAN_METHOD(MakeFunction);
    static NAN_METHOD(DeclareFunction);

    static NAN_METHOD(Constant);

public:
    CodeUnit *Unit;

    static inline Nan::Persistent<Function>& constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }

    static NAN_MODULE_INIT(Init);
};

#endif
