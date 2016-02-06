// type wrapper

#include "type_wrapper.h"

NAN_METHOD(TypeWrapper::New)
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

NAN_METHOD(TypeWrapper::ToString)
{
    TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(info.This());

    std::string name = wrapper->Type->toString();

    info.GetReturnValue().Set(Nan::New(name).ToLocalChecked());
}

Handle<Value> TypeWrapper::wrapType(TypeHandle *type)
{
    Nan::EscapableHandleScope scope;

    const unsigned argc = 1;
    Handle<Value> argv[1] = { Nan::New<External>((void *)type) };
    Local<Function> cons = Nan::New(constructor());

    return scope.Escape(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
}

NAN_MODULE_INIT(TypeWrapper::Init)
{
    Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(TypeWrapper::New);

    tmpl->SetClassName(Nan::New("Type").ToLocalChecked());
    tmpl->InstanceTemplate()->SetInternalFieldCount(1);
    Nan::SetPrototypeMethod(tmpl, "toString", TypeWrapper::ToString);

    constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
    Nan::Set(target, Nan::New("Type").ToLocalChecked(),
        Nan::GetFunction(tmpl).ToLocalChecked());

    prototype.Reset(tmpl);
}

NAN_METHOD(TypeWrapper::GetVoidTy)
{
    info.GetReturnValue().Set(wrapType(new VoidTypeHandle()));
}

NAN_METHOD(TypeWrapper::GetIntTy)
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

NAN_METHOD(TypeWrapper::GetFloatTy)
{

    if (info.Length() == 0 || !info[0]->IsNumber()) {
        return Nan::ThrowError("Must provide float bit width");
    }

    Local<Number> bitWidth = info[0].As<Number>();
    double requestedBits = bitWidth->Value();

    if (requestedBits != 16 && requestedBits != 32 && requestedBits != 64)
    {
        return Nan::ThrowError("Invalid float bit width");
    }

    unsigned bits = (unsigned)requestedBits;

    info.GetReturnValue().Set(wrapType(new FloatTypeHandle(bits)));
}

NAN_METHOD(TypeWrapper::GetPointerTy)
{
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
    TypeHandle *pointee = wrapper->Type;

    info.GetReturnValue().Set(wrapType(new PointerTypeHandle(pointee)));
}

NAN_METHOD(TypeWrapper::GetArrayTy)
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

NAN_METHOD(TypeWrapper::GetFunctionTy)
{
    EXTRACT_FUNCTION_PARAMS(0)

    info.GetReturnValue().Set(wrapType(new FunctionTypeHandle(returns, takes)));
}

Nan::Persistent<FunctionTemplate> TypeWrapper::prototype;
