// value wrapper

#include "value_wrapper.h"

#include "type_wrapper.h"

std::map<ValueHandle *, std::shared_ptr<ValueHandle>> ValueWrapper::value_cache;

NAN_METHOD(ValueWrapper::New)
{
    if (!info.IsConstructCall() || info.Length() == 0 || !info[0]->IsExternal())
    {
        return Nan::ThrowError("Cannot instantiate value directly, use factory");
    }

    Handle<External> handle = Handle<External>::Cast(info[0]);
    ValueHandle *v = static_cast<ValueHandle *>(handle->Value());
    std::shared_ptr<ValueHandle> val = value_cache[v];
    ValueWrapper *instance = new ValueWrapper(val);

    instance->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

NAN_GETTER(ValueWrapper::GetType)
{
    ValueWrapper *wrapper = Nan::ObjectWrap::Unwrap<ValueWrapper>(info.This());

    info.GetReturnValue().Set(TypeWrapper::wrapType(wrapper->Val->Type));
}

Handle<Value> ValueWrapper::wrapValue(std::shared_ptr<ValueHandle> value)
{
    ValueHandle *ptr = value.get();
    if (!value_cache.count(ptr)) {
      value_cache[ptr] = std::move(value);
    }

    Nan::EscapableHandleScope scope;

    const unsigned argc = 1;
    Handle<Value> argv[argc] = { Nan::New<External>((void *)ptr) };
    Local<Function> cons = Nan::New(constructor());

    return scope.Escape(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
}

NAN_MODULE_INIT(ValueWrapper::Init)
{
    Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(ValueWrapper::New);

    tmpl->SetClassName(Nan::New("Value").ToLocalChecked());
    tmpl->InstanceTemplate()->SetInternalFieldCount(1);
    Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("type").ToLocalChecked(), GetType);

    constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
    Nan::Set(target, Nan::New("Value").ToLocalChecked(),
        Nan::GetFunction(tmpl).ToLocalChecked());

    prototype.Reset(tmpl);
}

Nan::Persistent<FunctionTemplate> ValueWrapper::prototype;
