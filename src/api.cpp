// codegen

#include <nan.h>
using Nan::ObjectWrap;

using namespace v8;

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "types.h"
#include "function_builder.h"
#include "codegen.h"

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

    static NAN_METHOD(New)
    {

        if (!info.IsConstructCall() || info.Length() == 0 || !info[0]->IsExternal())
        {
            return Nan::ThrowError("Cannot instantiate type directly, use factory");
        }

        Handle<External> handle = Handle<External>::Cast(info[0]);
        TypeHandle *t = static_cast<TypeHandle *>(handle->Value());
        TypeWrapper *instance = new TypeWrapper(t);

        instance->Wrap(info.This());

        info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(ToString)
    {
        TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(info.This());

        std::string name = wrapper->Type->toString();

        info.GetReturnValue().Set(Nan::New(name).ToLocalChecked());
    }

public:
    static Handle<Value> wrapType(TypeHandle *type)
    {
        Nan::EscapableHandleScope scope;

        const unsigned argc = 1;
        Handle<Value> argv[1] = { Nan::New<External>((void *)type) };
        Local<Function> cons = Nan::New(constructor());

        return scope.Escape(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }

    TypeHandle *Type;

    static Nan::Persistent<FunctionTemplate> prototype;

    static inline Nan::Persistent<Function>& constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }

    static NAN_MODULE_INIT(Init) {
        Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(TypeWrapper::New);

        tmpl->SetClassName(Nan::New("Type").ToLocalChecked());
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);
        Nan::SetPrototypeMethod(tmpl, "toString", TypeWrapper::ToString);

        constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
        Nan::Set(target, Nan::New("TypeWrapper").ToLocalChecked(),
            Nan::GetFunction(tmpl).ToLocalChecked());

        prototype.Reset(tmpl);
    }

    static NAN_METHOD(GetVoidTy)
    {
        info.GetReturnValue().Set(wrapType(new VoidTypeHandle()));
    }

    static NAN_METHOD(GetIntTy)
    {

        if (info.Length() == 0 || !info[0]->IsNumber()) {
            return Nan::ThrowError("Must provide integer bit width");
        }

        Local<Number> bitWidth = info[0].As<Number>();
        double requestedBits = bitWidth->Value();

        if (requestedBits < MIN_INT_BITS) {
            return Nan::ThrowError("Integer bit width below the minimum");
        }

        if (requestedBits > MAX_INT_BITS) {
            return Nan::ThrowError("Integer bit width above the maximum");
        }

        unsigned bits = (unsigned)requestedBits;

        if (bits != requestedBits) {
            return Nan::ThrowError("Integer bit width not valid");
        }

        info.GetReturnValue().Set(wrapType(new IntTypeHandle(bits)));
    }

    static NAN_METHOD(GetPointerTy)
    {

        TypeHandle *pointee;

        if (info.Length() == 0)
        {
            return Nan::ThrowError("Must provide pointee type");
        }

        Local<Object> handle = info[0]->ToObject();

        if (!Nan::New(prototype)->HasInstance(handle))
        {
            return Nan::ThrowError("Argument must be a type specifier");
        }

        TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(handle);
        pointee = wrapper->Type;

        info.GetReturnValue().Set(wrapType(new PointerTypeHandle(pointee)));
    }

    static NAN_METHOD(GetArrayTy)
    {

        unsigned size;
        TypeHandle *element;

        if (info.Length() == 0 || !info[0]->IsNumber())
        {
            return Nan::ThrowError("Must provide array size");
        }

        Local<Number> sizeNumber = info[0].As<Number>();
        double sizeDouble = sizeNumber->Value();
        size = (unsigned)sizeDouble;

        if (info.Length() == 1)
        {
            return Nan::ThrowError("Must provide array element type");
        }

        Local<Object> handle = info[1]->ToObject();

        if (!Nan::New(prototype)->HasInstance(handle))
        {
            return Nan::ThrowError("Argument must be a type specifier");
        }

        TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(handle);
        element = wrapper->Type;

        info.GetReturnValue().Set(wrapType(new ArrayTypeHandle(size, element)));
    }

    static NAN_METHOD(GetFunctionTy)
    {
        EXTRACT_FUNCTION_PARAMS(0)

        info.GetReturnValue().Set(wrapType(new FunctionTypeHandle(returns, takes)));
    }
};

Nan::Persistent<FunctionTemplate> TypeWrapper::prototype;

class ValueWrapper : public Nan::ObjectWrap
{
    ValueWrapper(ValueHandle *v)
    : Val(v) {}

    static NAN_METHOD(New)
    {

        if (!info.IsConstructCall() || info.Length() == 0 || !info[0]->IsExternal())
        {
            return Nan::ThrowError("Cannot instantiate value directly, use factory");
        }

        Handle<External> handle = Handle<External>::Cast(info[0]);
        ValueHandle *v = static_cast<ValueHandle *>(handle->Value());
        ValueWrapper *instance = new ValueWrapper(v);

        instance->Wrap(info.This());

        info.GetReturnValue().Set(info.This());
    }

    static NAN_GETTER(GetType)
    {
        ValueWrapper *wrapper = Nan::ObjectWrap::Unwrap<ValueWrapper>(info.This());

        info.GetReturnValue().Set(TypeWrapper::wrapType(wrapper->Val->Type));
    }

public:
    static Handle<Value> wrapValue(ValueHandle *value)
    {
        Nan::EscapableHandleScope scope;

        const unsigned argc = 1;
        Handle<Value> argv[argc] = { Nan::New<External>((void *)value) };
        Local<Function> cons = Nan::New(constructor());

        return scope.Escape(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }

    ValueHandle *Val;

    static Nan::Persistent<FunctionTemplate> prototype;

    static inline Nan::Persistent<Function>& constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }

    static NAN_MODULE_INIT(Init)
    {
        Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(ValueWrapper::New);

        tmpl->SetClassName(Nan::New("Value").ToLocalChecked());
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);
        Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("type").ToLocalChecked(), GetType);

        constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
        Nan::Set(target, Nan::New("ValueWrapper").ToLocalChecked(),
            Nan::GetFunction(tmpl).ToLocalChecked());

        prototype.Reset(tmpl);
    }
};

Nan::Persistent<FunctionTemplate> ValueWrapper::prototype;

class FunctionBuilderWrapper;

bool isFunctionWrapper(Local<v8::Object> obj);
FunctionBuilder *getFunctionBuilder(FunctionBuilderWrapper *wrapper);

class BuilderWrapper : public Nan::ObjectWrap
{
protected:

    explicit BuilderWrapper(InstructionBuilder *builder)
    : Builder(builder) {}

    static NAN_METHOD(New)
    {
        if (!info.IsConstructCall() || info.Length() == 0 || !info[0]->IsExternal())
        {
            return Nan::ThrowError("Cannot instantiate type directly, use factory");
        }

        Handle<External> handle = Handle<External>::Cast(info[0]);
        InstructionBuilder *b = static_cast<InstructionBuilder *>(handle->Value());
        BuilderWrapper *instance = new BuilderWrapper(b);

        instance->Wrap(info.This());

        info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Alloca)
    {
        BuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

        if (info.Length() == 0)
        {
            return Nan::ThrowError("Alloca type required");
        }

        if (!Nan::New(TypeWrapper::prototype)->HasInstance(info[0]))
        {
            return Nan::ThrowError("Alloca type required");
        }

        TypeWrapper *t = Nan::ObjectWrap::Unwrap<TypeWrapper>(info[0].As<Object>());

        ValueHandle *h = 0;

        if (info.Length() == 1)
        {
            h = wrapper->Builder->Alloca(t->Type);
        }
        else if (info[1]->IsNumber())
        {
            Local<Number> num = info[1].As<Number>();
            double numVal = num->Value();

            h = wrapper->Builder->Alloca(t->Type, (int)numVal);
        }
        else if (Nan::New(ValueWrapper::prototype)->HasInstance(info[1]))
        {
            ValueWrapper *value = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[1].As<Object>());

            if (!value->Val->Type->isIntType())
            {
                return Nan::ThrowError("Alloca array size must be an integer value");
            }

            h = wrapper->Builder->Alloca(t->Type, value->Val);
        }
        else
        {
            return Nan::ThrowError("Alloca array size type not supported");
        }

        info.GetReturnValue().Set(ValueWrapper::wrapValue(h));
    }

    static NAN_METHOD(Load)
    {
        BuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());
        
        if (info.Length() < 1)
        {
            return Nan::ThrowError("Load requires a pointer to load from");
        }

        if (!Nan::New(ValueWrapper::prototype)->HasInstance(info[0]))
        {
            return Nan::ThrowError("Load requires a pointer to load from");
        }

        ValueWrapper *ptr = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());

        if (!ptr->Val->Type->isPointerType())
        {
            return Nan::ThrowError("Load requires pointer type");
        }

        ValueHandle *load = wrapper->Builder->Load(ptr->Val);
        
        info.GetReturnValue().Set(ValueWrapper::wrapValue(load));
    }

    static NAN_METHOD(Store)
    {
        BuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

        if (info.Length() < 2)
        {
            return Nan::ThrowError("Store requires a value and a pointer to store");
        }

        if (!Nan::New(ValueWrapper::prototype)->HasInstance(info[1]))
        {
            return Nan::ThrowError("Store requires a pointer to store to");
        }

        ValueWrapper *ptr = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[1].As<Object>());

        if (!ptr->Val->Type->isPointerType())
        {
            return Nan::ThrowError("Store requires pointer type");
        }

        PointerTypeHandle *pt = static_cast<PointerTypeHandle *>(ptr->Val->Type);

        if (info[0]->IsNumber())
        {
            if (!pt->pointee->isIntType())
            {
                return Nan::ThrowError("Store type mismatch");
            }

            Local<Number> num = info[0].As<Number>();
            double numVal = num->Value();

            wrapper->Builder->Store((int)numVal, ptr->Val);
        }
        else if (Nan::New(ValueWrapper::prototype)->HasInstance(info[0]))
        {
            ValueWrapper *value = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());

            // TODO: unify types

            wrapper->Builder->Store(value->Val, ptr->Val);
        }
        else
        {
            return Nan::ThrowError("Store value type not supported");
        }

        info.GetReturnValue().Set(info.This());
    }

#define BINARY_METHOD(name) \
    static NAN_METHOD(name) \
    { \
        BuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This()); \
\
        if (info.Length() < 2) \
        { \
            return Nan::ThrowError("Two values required"); \
        } \
\
        Local<FunctionTemplate> val = Nan::New(ValueWrapper::prototype); \
\
        if (!val->HasInstance(info[0]) || !val->HasInstance(info[1])) \
        { \
            return Nan::ThrowError("Two values required"); \
        } \
\
        ValueWrapper *lhs = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>()); \
        ValueWrapper *rhs = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[1].As<Object>()); \
\
        ValueHandle *result = wrapper->Builder->name(lhs->Val, rhs->Val);\
\
        info.GetReturnValue().Set(ValueWrapper::wrapValue(result));\
    }

    BINARY_METHOD(Add)
    BINARY_METHOD(Sub)
    BINARY_METHOD(Mul)
    BINARY_METHOD(UDiv)
    BINARY_METHOD(SDiv)
    BINARY_METHOD(URem)
    BINARY_METHOD(SRem)
    BINARY_METHOD(And)
    BINARY_METHOD(Or)
    BINARY_METHOD(Xor)
    BINARY_METHOD(Shl)
    BINARY_METHOD(LShr)
    BINARY_METHOD(AShr)

    BINARY_METHOD(Equal)
    BINARY_METHOD(NotEqual)
    BINARY_METHOD(UGreaterThan)
    BINARY_METHOD(UAtLeast)
    BINARY_METHOD(ULessThan)
    BINARY_METHOD(UAtMost)
    BINARY_METHOD(SGreaterThan)
    BINARY_METHOD(SAtLeast)
    BINARY_METHOD(SLessThan)
    BINARY_METHOD(SAtMost)

    static NAN_METHOD(Select)
    {
        BuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

        if (info.Length() < 3)
        {
            return Nan::ThrowError("Select requires three values");
        }

        Local<FunctionTemplate> val = Nan::New(ValueWrapper::prototype);

        if (!val->HasInstance(info[0]) || !val->HasInstance(info[1]) || !val->HasInstance(info[2]))
        {
            return Nan::ThrowError("Select requires three values");
        }

        ValueWrapper *cond = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());
        ValueWrapper *ifTrue = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[1].As<Object>());
        ValueWrapper *ifFalse = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[2].As<Object>());

        ValueHandle *result = wrapper->Builder->Select(cond->Val, ifTrue->Val, ifFalse->Val);

        info.GetReturnValue().Set(ValueWrapper::wrapValue(result));
    }

    static NAN_METHOD(Value)
    {
        BuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

        if (info.Length() < 2)
        {
            return Nan::ThrowError("Value type and value required");
        }

        if (!Nan::New(TypeWrapper::prototype)->HasInstance(info[0]))
        {
            return Nan::ThrowError("Value requires a type");
        }

        TypeWrapper *type = Nan::ObjectWrap::Unwrap<TypeWrapper>(info[0].As<Object>());

        ValueHandle *result;

        if (info[1]->IsNumber())
        {
            Local<Number> num = info[1].As<Number>();
            double numVal = num->Value();

            result = wrapper->Builder->MakeValue(type->Type, (int)numVal);
        }
        else
        {
            return Nan::ThrowError("Value type not supported");
        }

        info.GetReturnValue().Set(ValueWrapper::wrapValue(result));
    }

    static NAN_METHOD(Return)
    {

        BuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

        if (info.Length() == 0)
        {
            wrapper->Builder->Return();
        }
        else if (info[0]->IsNumber())
        {
            Local<Number> num = info[0].As<Number>();
            double numVal = num->Value();

            wrapper->Builder->Return((int)numVal);
        }
        else if (Nan::New(ValueWrapper::prototype)->HasInstance(info[0]))
        {
            ValueWrapper *value = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());

            // TODO: unify types

            wrapper->Builder->Return(value->Val);
        }
        else
        {
            return Nan::ThrowError("Return value type not supported");
        }

        info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Parameter)
    {
        BuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

        if (info.Length() == 0 || !(info[0]->IsNumber()))
        {
            return Nan::ThrowError("Must provide parameter index");
        }

        Local<Number> num = info[0].As<Number>();
        double numVal = num->Value();

        ValueHandle *val = wrapper->Builder->Parameter((size_t)numVal);

        if (val == 0)
        {
            return Nan::ThrowError("Parameter index invalid");
        }

        info.GetReturnValue().Set(ValueWrapper::wrapValue(val));
    }

    static NAN_METHOD(LoadConstant)
    {

        BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

        if (info.Length() == 0)
        {
            return Nan::ThrowError("Must provide constant value");
        }

        Local<Object> handle = info[0]->ToObject();

        if (!Nan::New(ValueWrapper::prototype)->HasInstance(handle))
        {
            return Nan::ThrowError("Must provide constant value");
        }

        ValueWrapper *wrapper = Nan::ObjectWrap::Unwrap<ValueWrapper>(handle);

        ValueHandle *result = self->Builder->LoadConstant(wrapper->Val);

        info.GetReturnValue().Set(ValueWrapper::wrapValue(result));
    }

    static NAN_METHOD(CallFunction)
    {

        BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

        if (info.Length() == 0)
        {
            return Nan::ThrowError("Must provide function to call");
        }

        std::vector<ValueHandle *> argVals;

        for (unsigned i = 1, e = info.Length(); i < e; i += 1)
        {
            Local<Object> handle = info[i]->ToObject();

            if (!Nan::New(ValueWrapper::prototype)->HasInstance(handle))
            {
                return Nan::ThrowError("Argument must be a value");
            }

            ValueWrapper *arg = Nan::ObjectWrap::Unwrap<ValueWrapper>(handle);
            argVals.push_back(arg->Val);
        }

        Local<Object> handle = info[0]->ToObject();

        ValueHandle *result;

        if (Nan::New(ValueWrapper::prototype)->HasInstance(handle))
        {
            ValueWrapper *wrapper = Nan::ObjectWrap::Unwrap<ValueWrapper>(handle);
            ValueHandle *callee = wrapper->Val;

            if (!callee->Type->isFunctionType())
            {
                return Nan::ThrowError("Only function values are callable");
            }

            result = self->Builder->CallFunction(callee, argVals);
        }
        else if (isFunctionWrapper(handle))
        {
            FunctionBuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<FunctionBuilderWrapper>(handle);
            FunctionBuilder *callee = getFunctionBuilder(wrapper);

            result = self->Builder->CallFunction(callee, argVals);
        }
        else
        {
            return Nan::ThrowError("Must provide function value");
        }

        info.GetReturnValue().Set(ValueWrapper::wrapValue(result));
    }

public:
    static Handle<v8::Value> wrapBuilder(InstructionBuilder *value)
    {
        Nan::EscapableHandleScope scope;

        const unsigned argc = 1;
        Handle<v8::Value> argv[argc] = { Nan::New<External>((void *)value) };
        Local<Function> cons = Nan::New(constructor());

        return scope.Escape(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }

    InstructionBuilder *Builder;

    static Nan::Persistent<FunctionTemplate> prototype;

    static inline Nan::Persistent<Function>& constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }

    static NAN_MODULE_INIT(Init)
    {
        Nan::HandleScope scope;

        Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(BuilderWrapper::New);

        tmpl->SetClassName(Nan::New("Builder").ToLocalChecked());
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);

        Nan::SetPrototypeMethod(tmpl, "return", Return);
        Nan::SetPrototypeMethod(tmpl, "parameter", Parameter);
        Nan::SetPrototypeMethod(tmpl, "loadConstant", LoadConstant);
        Nan::SetPrototypeMethod(tmpl, "callFunction", CallFunction);
        Nan::SetPrototypeMethod(tmpl, "alloca", Alloca);
        Nan::SetPrototypeMethod(tmpl, "load", Load);
        Nan::SetPrototypeMethod(tmpl, "store", Store);
        Nan::SetPrototypeMethod(tmpl, "add", Add);
        Nan::SetPrototypeMethod(tmpl, "sub", Sub);
        Nan::SetPrototypeMethod(tmpl, "mul", Mul);
        Nan::SetPrototypeMethod(tmpl, "udiv", UDiv);
        Nan::SetPrototypeMethod(tmpl, "sdiv", SDiv);
        Nan::SetPrototypeMethod(tmpl, "urem", URem);
        Nan::SetPrototypeMethod(tmpl, "srem", SRem);
        Nan::SetPrototypeMethod(tmpl, "and", And);
        Nan::SetPrototypeMethod(tmpl, "or", Or);
        Nan::SetPrototypeMethod(tmpl, "xor", Xor);
        Nan::SetPrototypeMethod(tmpl, "shl", Shl);
        Nan::SetPrototypeMethod(tmpl, "lshr", LShr);
        Nan::SetPrototypeMethod(tmpl, "ashr", AShr);

        Nan::SetPrototypeMethod(tmpl, "equal", Equal);
        Nan::SetPrototypeMethod(tmpl, "notEqual", NotEqual);
        Nan::SetPrototypeMethod(tmpl, "uGreaterThan", UGreaterThan);
        Nan::SetPrototypeMethod(tmpl, "uAtLeast", UAtLeast);
        Nan::SetPrototypeMethod(tmpl, "uLessThan", ULessThan);
        Nan::SetPrototypeMethod(tmpl, "uAtMost", UAtMost);
        Nan::SetPrototypeMethod(tmpl, "sGreaterThan", UGreaterThan);
        Nan::SetPrototypeMethod(tmpl, "sAtLeast", UAtLeast);
        Nan::SetPrototypeMethod(tmpl, "sLessThan", ULessThan);
        Nan::SetPrototypeMethod(tmpl, "sAtMost", UAtMost);

        Nan::SetPrototypeMethod(tmpl, "select", Select);

        Nan::SetPrototypeMethod(tmpl, "value", Value);

        constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
        Nan::Set(target, Nan::New("Builder").ToLocalChecked(),
            Nan::GetFunction(tmpl).ToLocalChecked());

        prototype.Reset(tmpl);
    }
};

Nan::Persistent<FunctionTemplate> BuilderWrapper::prototype;

class FunctionBuilderWrapper : public BuilderWrapper
{
    explicit FunctionBuilderWrapper(FunctionBuilder *builder)
    : BuilderWrapper(builder) {}

    static NAN_METHOD(New)
    {
        if (!info.IsConstructCall() || info.Length() == 0 || !info[0]->IsExternal())
        {
            return Nan::ThrowError("Cannot instantiate type directly, use factory");
        }

        Handle<External> handle = Handle<External>::Cast(info[0]);
        FunctionBuilder *fb = static_cast<FunctionBuilder *>(handle->Value());
        FunctionBuilderWrapper *instance = new FunctionBuilderWrapper(fb);

        instance->Wrap(info.This());

        info.GetReturnValue().Set(info.This());
    }

    static NAN_GETTER(GetName)
    {
        FunctionBuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<FunctionBuilderWrapper>(info.This());

        FunctionBuilder *fn = getFunctionBuilder(wrapper);

        info.GetReturnValue().Set(Nan::New(fn->Name).ToLocalChecked());
    }

    static NAN_GETTER(GetType)
    {
        FunctionBuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<FunctionBuilderWrapper>(info.This());

        FunctionBuilder *fn = getFunctionBuilder(wrapper);

        info.GetReturnValue().Set(TypeWrapper::wrapType(fn->Type));
    }

public:
    static Nan::Persistent<FunctionTemplate> prototype;

    static inline Nan::Persistent<Function>& constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }

    static NAN_MODULE_INIT(Init)
    {
        Nan::HandleScope scope;

        Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(FunctionBuilderWrapper::New);

        tmpl->SetClassName(Nan::New("FunctionBuilder").ToLocalChecked());
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);
        Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("name").ToLocalChecked(), GetName);
        Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("type").ToLocalChecked(), GetType);

        Nan::SetPrototypeMethod(tmpl, "return", Return);
        Nan::SetPrototypeMethod(tmpl, "parameter", Parameter);
        Nan::SetPrototypeMethod(tmpl, "loadConstant", LoadConstant);
        Nan::SetPrototypeMethod(tmpl, "callFunction", CallFunction);
        Nan::SetPrototypeMethod(tmpl, "alloca", Alloca);
        Nan::SetPrototypeMethod(tmpl, "load", Load);
        Nan::SetPrototypeMethod(tmpl, "store", Store);
        Nan::SetPrototypeMethod(tmpl, "add", Add);
        Nan::SetPrototypeMethod(tmpl, "sub", Sub);
        Nan::SetPrototypeMethod(tmpl, "mul", Mul);
        Nan::SetPrototypeMethod(tmpl, "udiv", UDiv);
        Nan::SetPrototypeMethod(tmpl, "sdiv", SDiv);
        Nan::SetPrototypeMethod(tmpl, "urem", URem);
        Nan::SetPrototypeMethod(tmpl, "srem", SRem);
        Nan::SetPrototypeMethod(tmpl, "and", And);
        Nan::SetPrototypeMethod(tmpl, "or", Or);
        Nan::SetPrototypeMethod(tmpl, "xor", Xor);
        Nan::SetPrototypeMethod(tmpl, "shl", Shl);
        Nan::SetPrototypeMethod(tmpl, "lshr", LShr);
        Nan::SetPrototypeMethod(tmpl, "ashr", AShr);

        Nan::SetPrototypeMethod(tmpl, "equal", Equal);
        Nan::SetPrototypeMethod(tmpl, "notEqual", NotEqual);
        Nan::SetPrototypeMethod(tmpl, "uGreaterThan", UGreaterThan);
        Nan::SetPrototypeMethod(tmpl, "uAtLeast", UAtLeast);
        Nan::SetPrototypeMethod(tmpl, "uLessThan", ULessThan);
        Nan::SetPrototypeMethod(tmpl, "uAtMost", UAtMost);
        Nan::SetPrototypeMethod(tmpl, "sGreaterThan", UGreaterThan);
        Nan::SetPrototypeMethod(tmpl, "sAtLeast", UAtLeast);
        Nan::SetPrototypeMethod(tmpl, "sLessThan", ULessThan);
        Nan::SetPrototypeMethod(tmpl, "sAtMost", UAtMost);

        Nan::SetPrototypeMethod(tmpl, "select", Select);

        Nan::SetPrototypeMethod(tmpl, "value", Value);

        constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
        Nan::Set(target, Nan::New("FunctionBuilder").ToLocalChecked(),
            Nan::GetFunction(tmpl).ToLocalChecked());

        prototype.Reset(tmpl);
    }
};

Nan::Persistent<FunctionTemplate> FunctionBuilderWrapper::prototype;

bool isFunctionWrapper(Local<v8::Object> obj)
{
    return Nan::New(FunctionBuilderWrapper::prototype)->HasInstance(obj);
}

FunctionBuilder *getFunctionBuilder(FunctionBuilderWrapper *wrapper)
{
    return static_cast<FunctionBuilder *>(wrapper->Builder);
}

class CodeUnitWrapper : public Nan::ObjectWrap
{
    explicit CodeUnitWrapper(CodeUnit *unit)
    : Unit(unit) {}

    static NAN_METHOD(New)
    {
        if (!info.IsConstructCall())
        {
            const int argc = 1;
            Local<Value> argv[argc] = { info[0] };
            Local<Function> cons = Nan::New(constructor());
            info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
        }
        else
        {
            if (!info[0]->IsString())
            {
                return Nan::ThrowError("Must provide file name.");
            }

            Local<String> filename = info[0].As<String>();
            String::Utf8Value encoded(filename);

            CodeUnit *unit = new CodeUnit(*encoded);
            CodeUnitWrapper *wrapper = new CodeUnitWrapper(unit);
            wrapper->Wrap(info.This());

            info.GetReturnValue().Set(info.This());
        }
    }

    void dumpModule()
    {
        Unit->dumpModule();
    }

    static NAN_METHOD(Dump)
    {

        CodeUnitWrapper *self = Nan::ObjectWrap::Unwrap<CodeUnitWrapper>(info.This());

        self->dumpModule();

        return;
    }

    static NAN_METHOD(WriteToFile)
    {

        if (info.Length() == 0 || !info[0]->IsString())
        {
            return Nan::ThrowError("Must provide file name");
        }

        Local<String> raw = info[0].As<String>();
        String::Utf8Value filename(raw);

        CodeUnitWrapper *self = Nan::ObjectWrap::Unwrap<CodeUnitWrapper>(info.This());
        if (!self->Unit->WriteToFile(*filename))
        {
            return Nan::ThrowError("Unable to write file");
        }

        return;
    }

    static NAN_METHOD(MakeFunction)
    {
        Nan::EscapableHandleScope scope;

        CodeUnitWrapper *self = Nan::ObjectWrap::Unwrap<CodeUnitWrapper>(info.This());

        if (info.Length() == 0 || !info[0]->IsString())
        {
            return Nan::ThrowError("Must provide function name");
        }

        Local<String> fnname = info[0].As<String>();
        String::Utf8Value encoded(fnname);

        EXTRACT_FUNCTION_PARAMS(1)

        FunctionBuilder *builder = self->Unit->MakeFunction(*encoded, new FunctionTypeHandle(returns, takes));

        if (!builder)
        {
            return Nan::ThrowError("Unable to create function (is name unique?)");
        }

        const unsigned argc = 1;
        Handle<Value> argv[argc] = { Nan::New<External>((void *)builder) };
        Local<Function> cons = Nan::New(FunctionBuilderWrapper::constructor());

        info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }

    static NAN_METHOD(DeclareFunction)
    {
        Nan::EscapableHandleScope scope;

        CodeUnitWrapper *self = Nan::ObjectWrap::Unwrap<CodeUnitWrapper>(info.This());

        if (info.Length() == 0 || !info[0]->IsString())
        {
            return Nan::ThrowError("Must provide function name");
        }

        Local<String> fnname = info[0].As<String>();
        String::Utf8Value encoded(fnname);

        EXTRACT_FUNCTION_PARAMS(1)

        FunctionValueHandle *fn = self->Unit->DeclareFunction(*encoded, new FunctionTypeHandle(returns, takes));

        if (!fn)
        {
            return Nan::ThrowError("Unable to create function (is name unique?)");
        }

        info.GetReturnValue().Set(ValueWrapper::wrapValue(fn));
    }

    static NAN_METHOD(Constant)
    {

        CodeUnitWrapper *self = Nan::ObjectWrap::Unwrap<CodeUnitWrapper>(info.This());

        if (info.Length() == 0)
        {
            return Nan::ThrowError("Must provide constant value");
        }

        if (info[0]->IsString())
        {
            Local<String> str = info[0].As<String>();
            String::Utf8Value encoded(str);

            ConstantValueHandle *handle = self->Unit->ConstantString(*encoded);
            info.GetReturnValue().Set(ValueWrapper::wrapValue(handle));
        }
        else
        {
            return Nan::ThrowError("Constant type yet supported");
        }
    }

public:
    CodeUnit *Unit;

    static inline Nan::Persistent<Function>& constructor() {
        static Nan::Persistent<Function> my_constructor;
        return my_constructor;
    }

    static NAN_MODULE_INIT(Init)
    {
        Nan::HandleScope scope;

        Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(CodeUnitWrapper::New);

        tmpl->SetClassName(Nan::New("CodeUnit").ToLocalChecked());
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);

        Nan::SetPrototypeMethod(tmpl, "dump", Dump);
        Nan::SetPrototypeMethod(tmpl, "writeBitcodeToFile", WriteToFile);
        Nan::SetPrototypeMethod(tmpl, "makeFunction", MakeFunction);
        Nan::SetPrototypeMethod(tmpl, "declareFunction", DeclareFunction);
        Nan::SetPrototypeMethod(tmpl, "constant", Constant);

        constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
        Nan::Set(target, Nan::New("CodeUnit").ToLocalChecked(),
            Nan::GetFunction(tmpl).ToLocalChecked());
    }
};

static NAN_MODULE_INIT(Init)
{
    TypeWrapper::Init(target);
    ValueWrapper::Init(target);
    CodeUnitWrapper::Init(target);
    BuilderWrapper::Init(target);
    FunctionBuilderWrapper::Init(target);

    Local<FunctionTemplate> getVoidTy = Nan::New<FunctionTemplate>(TypeWrapper::GetVoidTy);
    Nan::Set(target, Nan::New("getVoidTy").ToLocalChecked(), Nan::GetFunction(getVoidTy).ToLocalChecked());

    Local<FunctionTemplate> getFunctionTy = Nan::New<FunctionTemplate>(TypeWrapper::GetFunctionTy);
    Nan::Set(target, Nan::New("getFunctionTy").ToLocalChecked(), Nan::GetFunction(getFunctionTy).ToLocalChecked());

    Local<FunctionTemplate> getIntTy = Nan::New<FunctionTemplate>(TypeWrapper::GetIntTy);
    Nan::Set(target, Nan::New("getIntTy").ToLocalChecked(), Nan::GetFunction(getIntTy).ToLocalChecked());

    Local<FunctionTemplate> getPointerTy = Nan::New<FunctionTemplate>(TypeWrapper::GetPointerTy);
    Nan::Set(target, Nan::New("getPointerTy").ToLocalChecked(), Nan::GetFunction(getPointerTy).ToLocalChecked());

    Local<FunctionTemplate> getArrayTy = Nan::New<FunctionTemplate>(TypeWrapper::GetArrayTy);
    Nan::Set(target, Nan::New("getArrayTy").ToLocalChecked(), Nan::GetFunction(getArrayTy).ToLocalChecked());

    Local<Function> codeUnit = Nan::New(CodeUnitWrapper::constructor());
    Nan::Set(target, Nan::New("CodeUnit").ToLocalChecked(), codeUnit);
}

NODE_MODULE(codegen, Init)
