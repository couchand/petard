// codegen

#include <nan.h>
using Nan::ObjectWrap;

using namespace v8;

#include <vector>

#include "types.h"
#include "function_builder.h"
#include "codegen.h"

#include "type_wrapper.h"
#include "value_wrapper.h"
#include "builder_wrapper.h"
#include "function_builder_wrapper.h"

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
