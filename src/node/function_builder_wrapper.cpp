// function builder

#include "function_builder_wrapper.h"

#include "type_wrapper.h"

NAN_METHOD(FunctionBuilderWrapper::New)
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

NAN_GETTER(FunctionBuilderWrapper::GetName)
{
    FunctionBuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<FunctionBuilderWrapper>(info.This());

    FunctionBuilder *fn = wrapper->getFunctionBuilder();

    info.GetReturnValue().Set(Nan::New(fn->Name).ToLocalChecked());
}

NAN_GETTER(FunctionBuilderWrapper::GetType)
{
    FunctionBuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<FunctionBuilderWrapper>(info.This());

    FunctionBuilder *fn = wrapper->getFunctionBuilder();

    info.GetReturnValue().Set(TypeWrapper::wrapType(fn->Type));
}

NAN_MODULE_INIT(FunctionBuilderWrapper::Init)
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

    Nan::SetPrototypeMethod(tmpl, "if", If);

    constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
    Nan::Set(target, Nan::New("FunctionBuilder").ToLocalChecked(),
        Nan::GetFunction(tmpl).ToLocalChecked());

    prototype.Reset(tmpl);
}

Nan::Persistent<FunctionTemplate> FunctionBuilderWrapper::prototype;
