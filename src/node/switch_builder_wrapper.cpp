// switch builder wrapper

#include "switch_builder_wrapper.h"
#include "builder_wrapper.h"

Nan::Persistent<FunctionTemplate> SwitchBuilderWrapper::prototype;

NAN_METHOD(SwitchBuilderWrapper::New)
{
    if (!info.IsConstructCall() || info.Length() == 0 || !info[0]->IsExternal())
    {
        return Nan::ThrowError("Cannot instantiate type directly, use factory");
    }

    Handle<External> handle = Handle<External>::Cast(info[0]);
    SwitchBuilder *t = static_cast<SwitchBuilder *>(handle->Value());
    SwitchBuilderWrapper *instance = new SwitchBuilderWrapper(t);

    instance->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

NAN_METHOD(SwitchBuilderWrapper::AddCase)
{
    SwitchBuilderWrapper *self = Nan::ObjectWrap::Unwrap<SwitchBuilderWrapper>(info.This());

    if (!info[0]->IsNumber())
    {
        return Nan::ThrowError("Switch case requires constant integer");
    }

    Local<Number> num = info[0].As<Number>();
    double numVal = num->Value();
    int intVal = (int)numVal;

    if (!Nan::New(BuilderWrapper::prototype)->HasInstance(info[1]))
    {
        return Nan::ThrowError("Switch case requires branch target");
    }
    BuilderWrapper *target = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info[1].As<Object>());

    self->Switch->AddCase(intVal, target->Builder);
}

Handle<v8::Value> SwitchBuilderWrapper::wrapSwitchBuilder(SwitchBuilder *value)
{
    Nan::EscapableHandleScope scope;

    const unsigned argc = 1;
    Handle<v8::Value> argv[argc] = { Nan::New<External>((void *)value) };
    Local<Function> cons = Nan::New(constructor());

    return scope.Escape(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
}

NAN_MODULE_INIT(SwitchBuilderWrapper::Init)
{
    Nan::HandleScope scope;

    Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(SwitchBuilderWrapper::New);

    tmpl->SetClassName(Nan::New("SwitchBuilder").ToLocalChecked());
    tmpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tmpl, "addCase", AddCase);

    constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
    Nan::Set(target, Nan::New("Builder").ToLocalChecked(),
        Nan::GetFunction(tmpl).ToLocalChecked());

    prototype.Reset(tmpl);
}
