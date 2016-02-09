// type wrapper

#ifndef TYPE_WRAPPER_H
#define TYPE_WRAPPER_H

#include <nan.h>
using Nan::ObjectWrap;

using namespace v8;

#include <vector>

#include "type.h"

// definitions from http://www.llvm.org/docs/doxygen/html/DerivedTypes_8h_source.html#l00046
#define MIN_INT_BITS 1
#define MAX_INT_BITS (1<<23)-1

// function type helper
#define EXTRACT_FUNCTION_PARAMS(first)                                        \
    TypeHandle *returns;                                                      \
    std::vector<TypeHandle *> takes;                                          \
                                                                              \
    if (info.Length() == first)                                               \
    {                                                                         \
        returns = new VoidTypeHandle();                                       \
    }                                                                         \
    else                                                                      \
    {                                                                         \
        Local<Object> handle = info[first]->ToObject();                       \
                                                                              \
        TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(handle);  \
        returns = wrapper->Type;                                              \
    }                                                                         \
                                                                              \
    for (unsigned i = first + 1, e = info.Length(); i < e; i += 1)            \
    {                                                                         \
        Local<Object> handle = info[i]->ToObject();                           \
                                                                              \
        TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(handle);  \
        takes.push_back(wrapper->Type);                                       \
    }

class TypeWrapper : public Nan::ObjectWrap
{
    TypeWrapper(TypeHandle *t)
    : Type(t) {}

    static NAN_METHOD(New);
    static NAN_METHOD(ToString);
    static NAN_METHOD(IsCompatibleWith);

    static NAN_METHOD(IsVoidType);
    static NAN_METHOD(IsIntType);
    static NAN_METHOD(IsFloatType);
    static NAN_METHOD(IsArrayType);
    static NAN_METHOD(IsVectorType);
    static NAN_METHOD(IsStructType);
    static NAN_METHOD(IsPointerType);
    static NAN_METHOD(IsFunctionType);

    static NAN_GETTER(GetBitwidth);
    static NAN_GETTER(GetSize);
    static NAN_GETTER(GetElement);
    static NAN_GETTER(GetElements);
    static NAN_GETTER(GetPointee);
    static NAN_GETTER(GetReturns);
    static NAN_GETTER(GetParameters);

public:
    static Handle<Value> wrapType(TypeHandle *type);

    TypeHandle *Type;

    static Nan::Persistent<FunctionTemplate> prototype;

    static inline Nan::Persistent<Function>& constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }

    static NAN_MODULE_INIT(Init);

    static NAN_METHOD(GetVoidTy);
    static NAN_METHOD(GetIntTy);
    static NAN_METHOD(GetFloatTy);
    static NAN_METHOD(GetArrayTy);
    static NAN_METHOD(GetVectorTy);
    static NAN_METHOD(GetStructTy);
    static NAN_METHOD(GetPointerTy);
    static NAN_METHOD(GetFunctionTy);
};

#endif
