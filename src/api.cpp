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

        Handle<Value> argv[1] = { Nan::New<External>((void *)type) };

        return Nan::NewInstance(scope.Escape(Nan::New(constructor)));
    }

    TypeHandle *Type;

    static Nan::Persistent<FunctionTemplate> prototype;
    static Nan::Persistent<Function> constructor;

    static void Init()
    {
        Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(TypeWrapper::New);

        tmpl->SetClassName(Nan::New<String>("Type").ToLocalChecked());
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);
        Nan::SetPrototypeMethod(tmpl, "toString", TypeWrapper::ToString);

        prototype.Reset(tmpl);
        constructor.Reset(tmpl);
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

        /* TODO
        if (!Nan::HasInstance(prototype, handle))
        {
            return Nan::ThrowError("Argument must be a type specifier");
        }
        */

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

        /* TODO
        if (!Nan::HasInstance(prototype, handle))
        {
            return Nan::ThrowError("Argument must be a type specifier");
        }
        */

        TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(handle);
        element = wrapper->Type;

        info.GetReturnValue().Set(wrapType(new ArrayTypeHandle(size, element)));
    }


    static NAN_METHOD(GetFunctionTy)
    {

        TypeHandle *returns;
        std::vector<TypeHandle *> takes;

        if (info.Length() == 0)
        {
            returns = new VoidTypeHandle();
        }
        else
        {
            Local<Object> handle = info[0]->ToObject();

            /*
             * TODO
            if (!Nan::HasInstance(prototype, handle))
            {
                return Nan::ThrowError("Argument must be a type specifier");
            }
            */

            TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(handle);
            returns = wrapper->Type;
        }

        for (unsigned i = 1, e = info.Length(); i < e; i += 1)
        {
            Local<Object> handle = info[i]->ToObject();

            /* TODO
            if (!Nan::HasInstance(prototype, handle))
            {
                return Nan::ThrowError("Argument must be a type specifier");
            }
            */

            TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(handle);
            takes.push_back(wrapper->Type);
        }

        info.GetReturnValue().Set(wrapType(new FunctionTypeHandle(returns, takes)));
    }
};

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

public:
    static Handle<Value> wrapValue(ValueHandle *value)
    {
        Nan::EscapableHandleScope scope;

        Handle<Value> argv[1] = { Nan::New<External>((void *)value) };

        return Nan::NewInstance(scope.Escape(Nan::New(constructor)));
    }

    ValueHandle *Val;

    static Nan::Persistent<FunctionTemplate> prototype;
    static Nan::Persistent<Function> constructor;

    static void Init()
    {
        Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(ValueWrapper::New);

        tmpl->SetClassName(Nan::New<String>("Value").ToLocalChecked());
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);

        prototype.Reset(tmpl);
        Nan::GetFunction(constructor.Reset(tmpl));
    }
};

class FunctionBuilderWrapper : public Nan::ObjectWrap
{
    explicit FunctionBuilderWrapper(FunctionBuilder *builder)
    : Builder(builder) {}

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

        info.GetReturnValue().Set(wrapper->Builder->Name);
    }

    static NAN_GETTER(GetType)
    {

        FunctionBuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<FunctionBuilderWrapper>(info.This());

        info.GetReturnValue().Set(TypeWrapper::wrapType(wrapper->Builder->Type));
    }

    static NAN_METHOD(Return)
    {

        FunctionBuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<FunctionBuilderWrapper>(info.This());

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
        else
        {
            return Nan::ThrowError("Return value not supported");
        }

        info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(LoadConstant)
    {

        FunctionBuilderWrapper *self = Nan::ObjectWrap::Unwrap<FunctionBuilderWrapper>(info.This());

        if (info.Length() == 0)
        {
            return Nan::ThrowError("Must provide constant value");
        }

        Local<Object> handle = info[0]->ToObject();
        /* TODO
        if (!Nan::HasInstance(ValueWrapper::prototype, handle))
        {
            return Nan::ThrowError("Must provide constant value");
        }
        */

        ValueWrapper *wrapper = Nan::ObjectWrap::Unwrap<ValueWrapper>(handle);

        ValueHandle *result = self->Builder->LoadConstant(wrapper->Val);

        info.GetReturnValue().Set(ValueWrapper::wrapValue(result));
    }

    static NAN_METHOD(CallFunction)
    {

        FunctionBuilderWrapper *self = Nan::ObjectWrap::Unwrap<FunctionBuilderWrapper>(info.This());

        if (info.Length() == 0)
        {
            return Nan::ThrowError("Must provide function value");
        }

        Local<Object> handle = info[0]->ToObject();
        /* TODO
        if (!Nan::HasInstance(ValueWrapper::prototype, handle))
        {
            return Nan::ThrowError("Must provide function value");
        }
        */

        ValueWrapper *wrapper = Nan::ObjectWrap::Unwrap<ValueWrapper>(handle);
        ValueHandle *callee = wrapper->Val;

        std::vector<ValueHandle *> argVals;

        for (unsigned i = 1, e = info.Length(); i < e; i += 1)
        {
            Local<Object> handle = info[i]->ToObject();

            /* TODO
            if (!Nan::HasInstance(ValueWrapper::prototype, handle))
            {
                return Nan::ThrowError("Argument must be a value");
            }
            */

            ValueWrapper *arg = Nan::ObjectWrap::Unwrap<ValueWrapper>(handle);
            argVals.push_back(arg->Val);
        }

        ValueHandle *result = self->Builder->CallFunction(calleeals);

        info.GetReturnValue().Set(ValueWrapper::wrapValue(result));
    }

public:
    FunctionBuilder *Builder;

    static Nan::Persistent<FunctionTemplate> prototype;
    static Nan::Persistent<Function> constructor;

    static void Init()
    {
        Nan::Scope scope;

        Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(FunctionBuilderWrapper::New);

        tmpl->SetClassName(Nan::New("FunctionBuilder").ToLocalChecked());
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);
        Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("name").ToLocalChecked(), GetName);
        Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("type").ToLocalChecked(), GetType);

        Nan::SetPrototypeMethod(tmpl, "return", Return);
        Nan::SetPrototypeMethod(tmpl, "loadConstant", LoadConstant);
        Nan::SetPrototypeMethod(tmpl, "callFunction", CallFunction);

        prototype.Reset(tmpl);
        Nan::GetFunction(constructor.Reset(tmpl));
    }
};

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
            Local<Function> cons = Nan::New(constructor);
            Nan::NewInstance(info.GetReturnValue().Set(consargc));
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

        if (info.Length() == 1)
        {
            return Nan::ThrowError("Must provide function type");
        }

        Local<Object> handle = info[1]->ToObject();
        /* TODO
        if (!Nan::HasInstance(TypeWrapper::prototype, handle))
        {
            return Nan::ThrowError("Must provide function type");
        }
        */

        TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(handle);

        FunctionBuilder *builder = self->Unit->MakeFunction(*encoded, wrapper->Type);

        if (!builder)
        {
            return Nan::ThrowError("Unable to create function (is name unique?)");
        }

        Handle<Value> argv[1] = { Nan::New<External>((void *)builder) };

        Nan::NewInstance(info.GetReturnValue().Set(Nan::New(FunctionBuilderWrapper::constructor)));
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

        if (info.Length() == 1)
        {
            return Nan::ThrowError("Must provide function type");
        }

        Local<Object> handle = info[1]->ToObject();
        /* TODO
        if (!Nan::HasInstance(TypeWrapper::prototype, handle))
        {
            return Nan::ThrowError("Must provide function type");
        }
        */

        TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(handle);

        FunctionValueHandle *fn = self->Unit->DeclareFunction(*encoded, wrapper->Type);

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

    static Nan::Persistent<FunctionTemplate> prototype;
    static Nan::Persistent<Function> constructor;

    static void Init()
    {
        Nan::Scope scope;

        Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(CodeUnitWrapper::New);

        tmpl->SetClassName(Nan::New<String>("CodeUnit").ToLocalChecked());
        tmpl->InstanceTemplate()->SetInternalFieldCount(1);

        Nan::SetPrototypeMethod(tmpl, "dump", Dump);
        Nan::SetPrototypeMethod(tmpl, "writeBitcodeToFile", WriteToFile);
        Nan::SetPrototypeMethod(tmpl, "makeFunction", MakeFunction);
        Nan::SetPrototypeMethod(tmpl, "declareFunction", DeclareFunction);
        Nan::SetPrototypeMethod(tmpl, "constant", Constant);

        prototype.Reset(tmpl);
        Nan::GetFunction(constructor.Reset(tmpl));
    }
};

Nan::Persistent<FunctionTemplate> FunctionBuilderWrapper::prototype;
Nan::Persistent<Function> FunctionBuilderWrapper::constructor;
Nan::Persistent<FunctionTemplate> TypeWrapper::prototype;
Nan::Persistent<Function> TypeWrapper::constructor;
Nan::Persistent<FunctionTemplate> ValueWrapper::prototype;
Nan::Persistent<Function> ValueWrapper::constructor;
Nan::Persistent<FunctionTemplate> CodeUnitWrapper::prototype;
Nan::Persistent<Function> CodeUnitWrapper::constructor;

void Init(Handle<Object> exports, Handle<Object> module)
{
    TypeWrapper::Init();
    ValueWrapper::Init();
    CodeUnitWrapper::Init();
    FunctionBuilderWrapper::Init();

    Local<Function> getVoidTy = Nan::New<Function>(TypeWrapper::GetVoidTy);
    Nan::Set(exports, Nan::New<String>("getVoidTy").ToLocalChecked(), getVoidTy);

    Local<Function> getFunctionTy = Nan::New<Function>(TypeWrapper::GetFunctionTy);
    Nan::Set(exports, Nan::New<String>("getFunctionTy").ToLocalChecked(), getFunctionTy);

    Local<Function> getIntTy = Nan::New<Function>(TypeWrapper::GetIntTy);
    Nan::Set(exports, Nan::New<String>("getIntTy").ToLocalChecked(), getIntTy);

    Local<Function> getPointerTy = Nan::New<Function>(TypeWrapper::GetPointerTy);
    Nan::Set(exports, Nan::New<String>("getPointerTy").ToLocalChecked(), getPointerTy);

    Local<Function> getArrayTy = Nan::New<Function>(TypeWrapper::GetArrayTy);
    Nan::Set(exports, Nan::New<String>("getArrayTy").ToLocalChecked(), getArrayTy);

    Local<Function> codeUnit = Nan::New(CodeUnitWrapper::constructor);
    Nan::Set(exports, Nan::New<String>("CodeUnit").ToLocalChecked(), codeUnit);
}

NODE_MODULE(codegen, Init)
