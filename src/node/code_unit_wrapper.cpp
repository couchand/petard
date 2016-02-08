// codeunit

#include "code_unit_wrapper.h"

#include "function_builder.h"
#include "type_wrapper.h"
#include "value_wrapper.h"
#include "function_builder_wrapper.h"

#include "nan_macros.h"

NAN_METHOD(CodeUnitWrapper::New)
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

NAN_METHOD(CodeUnitWrapper::Dump)
{

    CodeUnitWrapper *self = Nan::ObjectWrap::Unwrap<CodeUnitWrapper>(info.This());

    self->Unit->dumpModule();

    return;
}

void doNotFree(char *data, void *hint) {}

NAN_METHOD(CodeUnitWrapper::JITFunction)
{
    CodeUnitWrapper *self = Nan::ObjectWrap::Unwrap<CodeUnitWrapper>(info.This());

    EXPECT_PARAM("JITFunction", 0, FunctionBuilderWrapper, "function to jit")
    FunctionBuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<FunctionBuilderWrapper>(info[0].As<Object>());
    FunctionBuilder *fn = wrapper->getFunctionBuilder();

    void *jitted = self->Unit->JITFunction(fn);

    info.GetReturnValue().Set(Nan::NewBuffer((char *)jitted, sizeof(void (*)()), &doNotFree, 0).ToLocalChecked());
}

NAN_METHOD(CodeUnitWrapper::WriteToFile)
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

NAN_METHOD(CodeUnitWrapper::MakeFunction)
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

NAN_METHOD(CodeUnitWrapper::DeclareFunction)
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

NAN_METHOD(CodeUnitWrapper::Constant)
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

NAN_MODULE_INIT(CodeUnitWrapper::Init)
{
    Nan::HandleScope scope;

    Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(CodeUnitWrapper::New);

    tmpl->SetClassName(Nan::New("CodeUnit").ToLocalChecked());
    tmpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tmpl, "dump", Dump);
    Nan::SetPrototypeMethod(tmpl, "jitFunction", JITFunction);
    Nan::SetPrototypeMethod(tmpl, "writeBitcodeToFile", WriteToFile);
    Nan::SetPrototypeMethod(tmpl, "makeFunction", MakeFunction);
    Nan::SetPrototypeMethod(tmpl, "declareFunction", DeclareFunction);
    Nan::SetPrototypeMethod(tmpl, "constant", Constant);

    constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
    Nan::Set(target, Nan::New("CodeUnit").ToLocalChecked(),
        Nan::GetFunction(tmpl).ToLocalChecked());
}
