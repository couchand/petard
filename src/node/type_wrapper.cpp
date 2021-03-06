// type wrapper

#include "type_wrapper.h"
#include "nan_macros.h"

std::map<const TypeHandle *, std::shared_ptr<const TypeHandle>> TypeWrapper::type_cache;

NAN_METHOD(TypeWrapper::New)
{
    if (!info.IsConstructCall() || info.Length() == 0 || !info[0]->IsExternal())
    {
        return Nan::ThrowError("Cannot instantiate type directly, use factory");
    }

    Handle<External> handle = Handle<External>::Cast(info[0]);
    const TypeHandle *t = static_cast<const TypeHandle *>(handle->Value());
    std::shared_ptr<const TypeHandle> ty = type_cache[t];
    TypeWrapper *instance = new TypeWrapper(ty);

    instance->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
}

NAN_METHOD(TypeWrapper::ToString)
{
    TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(info.This());

    std::string name = wrapper->Type->toString();

    info.GetReturnValue().Set(Nan::New(name).ToLocalChecked());
}

NAN_METHOD(TypeWrapper::IsCompatibleWith)
{
    TypeWrapper *self = Nan::ObjectWrap::Unwrap<TypeWrapper>(info.This());

    if (info.Length() == 0 || !Nan::New(prototype)->HasInstance(info[0]))
    {
        return Nan::ThrowError("Compatibility check requires a type handle");
    }

    TypeWrapper *other = Nan::ObjectWrap::Unwrap<TypeWrapper>(info[0].As<Object>());

    bool isCompatible = self->Type->isCompatibleWith(other->Type.get());
    info.GetReturnValue().Set(isCompatible);
}

#define TYPE_PREDICATE(name, pred) \
NAN_METHOD(TypeWrapper::name) \
{ \
    TypeWrapper *self = Nan::ObjectWrap::Unwrap<TypeWrapper>(info.This()); \
    info.GetReturnValue().Set(self->Type->pred()); \
}

TYPE_PREDICATE(IsVoidType, isVoidType);
TYPE_PREDICATE(IsIntType, isIntType);
TYPE_PREDICATE(IsFloatType, isFloatType);
TYPE_PREDICATE(IsArrayType, isArrayType);
TYPE_PREDICATE(IsVectorType, isVectorType);
TYPE_PREDICATE(IsStructType, isStructType);
TYPE_PREDICATE(IsPointerType, isPointerType);
TYPE_PREDICATE(IsFunctionType, isFunctionType);

#define RETURN_IF_TYPE(cls, pred, ret) \
    if (self->Type->pred()) \
    { \
        const cls *ty = static_cast<const cls *>(self->Type.get()); \
        info.GetReturnValue().Set(ty->ret); \
    }

#define RETURN_IF_TYPE_W(cls, pred, ret) \
    if (self->Type->pred()) \
    { \
        const cls *ty = static_cast<const cls *>(self->Type.get()); \
        info.GetReturnValue().Set(wrapType(ty->ret)); \
    }

#define RETURN_IF_TYPE_R(cls, pred, source) \
    if (self->Type->pred()) \
    { \
        const cls *ty = static_cast<const cls *>(self->Type.get()); \
\
        Local<Context> ctx = Nan::GetCurrentContext(); \
\
        Local<Array> types = Nan::New<Array>();\
        for (unsigned i = 0, e = ty->source.size(); i < e; i += 1) \
        { \
            types->Set(ctx, i, wrapType(ty->source[i])); \
        } \
\
        info.GetReturnValue().Set(types); \
    }

NAN_GETTER(TypeWrapper::GetBitwidth)
{
    TypeWrapper *self = Nan::ObjectWrap::Unwrap<TypeWrapper>(info.This());

    RETURN_IF_TYPE(IntTypeHandle, isIntType, numBits)
    RETURN_IF_TYPE(FloatTypeHandle, isFloatType, numBits)
}

NAN_GETTER(TypeWrapper::GetSize)
{
    TypeWrapper *self = Nan::ObjectWrap::Unwrap<TypeWrapper>(info.This());

    RETURN_IF_TYPE(ArrayTypeHandle, isArrayType, size)
    RETURN_IF_TYPE(VectorTypeHandle, isVectorType, size)
}

NAN_GETTER(TypeWrapper::GetElement)
{
    TypeWrapper *self = Nan::ObjectWrap::Unwrap<TypeWrapper>(info.This());

    RETURN_IF_TYPE_W(ArrayTypeHandle, isArrayType, element)
    RETURN_IF_TYPE_W(VectorTypeHandle, isVectorType, element)
}

NAN_GETTER(TypeWrapper::GetElements)
{
    Nan::EscapableHandleScope scope;

    TypeWrapper *self = Nan::ObjectWrap::Unwrap<TypeWrapper>(info.This());

    RETURN_IF_TYPE_R(StructTypeHandle, isStructType, elements)
}

NAN_GETTER(TypeWrapper::GetPointee)
{
    TypeWrapper *self = Nan::ObjectWrap::Unwrap<TypeWrapper>(info.This());

    RETURN_IF_TYPE_W(PointerTypeHandle, isPointerType, pointee)
}

NAN_GETTER(TypeWrapper::GetReturns)
{
    TypeWrapper *self = Nan::ObjectWrap::Unwrap<TypeWrapper>(info.This());

    RETURN_IF_TYPE_W(FunctionTypeHandle, isFunctionType, returns)
}

NAN_GETTER(TypeWrapper::GetParameters)
{
    Nan::EscapableHandleScope scope;

    TypeWrapper *self = Nan::ObjectWrap::Unwrap<TypeWrapper>(info.This());

    RETURN_IF_TYPE_R(FunctionTypeHandle, isFunctionType, params)
}

Handle<Value> TypeWrapper::wrapType(std::shared_ptr<const TypeHandle> type)
{
    const TypeHandle *ptr = type.get();
    if (!type_cache.count(ptr)) {
      type_cache[type.get()] = std::move(type);
    }

    Nan::EscapableHandleScope scope;

    const unsigned argc = 1;
    Handle<Value> argv[argc] = { Nan::New<External>((void *)ptr) };
    Local<Function> cons = Nan::New(constructor());

    return scope.Escape(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
}

NAN_MODULE_INIT(TypeWrapper::Init)
{
    Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(TypeWrapper::New);

    tmpl->SetClassName(Nan::New("Type").ToLocalChecked());
    tmpl->InstanceTemplate()->SetInternalFieldCount(1);
    Nan::SetPrototypeMethod(tmpl, "toString", TypeWrapper::ToString);
    Nan::SetPrototypeMethod(tmpl, "isCompatibleWith", TypeWrapper::IsCompatibleWith);

    Nan::SetPrototypeMethod(tmpl, "isVoidType", TypeWrapper::IsVoidType);
    Nan::SetPrototypeMethod(tmpl, "isIntType", TypeWrapper::IsIntType);
    Nan::SetPrototypeMethod(tmpl, "isFloatType", TypeWrapper::IsFloatType);
    Nan::SetPrototypeMethod(tmpl, "isArrayType", TypeWrapper::IsArrayType);
    Nan::SetPrototypeMethod(tmpl, "isVectorType", TypeWrapper::IsVectorType);
    Nan::SetPrototypeMethod(tmpl, "isStructType", TypeWrapper::IsStructType);
    Nan::SetPrototypeMethod(tmpl, "isPointerType", TypeWrapper::IsPointerType);
    Nan::SetPrototypeMethod(tmpl, "isFunctionType", TypeWrapper::IsFunctionType);

    Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("bitwidth").ToLocalChecked(), GetBitwidth);
    Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("size").ToLocalChecked(), GetSize);
    Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("element").ToLocalChecked(), GetElement);
    Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("elements").ToLocalChecked(), GetElements);
    Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("pointee").ToLocalChecked(), GetPointee);
    Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("returns").ToLocalChecked(), GetReturns);
    Nan::SetAccessor(tmpl->PrototypeTemplate(), Nan::New("parameters").ToLocalChecked(), GetParameters);

    constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
    Nan::Set(target, Nan::New("Type").ToLocalChecked(),
        Nan::GetFunction(tmpl).ToLocalChecked());

    prototype.Reset(tmpl);
}

NAN_METHOD(TypeWrapper::GetVoidTy)
{
    info.GetReturnValue().Set(wrapType(std::make_shared<VoidTypeHandle>()));
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

    info.GetReturnValue().Set(wrapType(std::make_shared<IntTypeHandle>(bits)));
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

    info.GetReturnValue().Set(wrapType(std::make_shared<FloatTypeHandle>(bits)));
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
    info.GetReturnValue().Set(wrapType(std::make_shared<PointerTypeHandle>(wrapper->Type)));
}

NAN_METHOD(TypeWrapper::GetVectorTy)
{
    unsigned size;

    if (info.Length() == 0 || !info[0]->IsNumber())
    {
        return Nan::ThrowError("Must provide vector size");
    }

    Local<Number> sizeNumber = info[0].As<Number>();
    double sizeDouble = sizeNumber->Value();
    size = (unsigned)sizeDouble;

    EXPECT_PARAM("GetVectorTy", 1, TypeWrapper, "element type")
    TypeWrapper *wrapper = Nan::ObjectWrap::Unwrap<TypeWrapper>(info[1]->ToObject());
    info.GetReturnValue().Set(wrapType(std::make_shared<VectorTypeHandle>(size, wrapper->Type)));
}

NAN_METHOD(TypeWrapper::GetArrayTy)
{
    unsigned size;

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
    info.GetReturnValue().Set(wrapType(std::make_shared<ArrayTypeHandle>(size, wrapper->Type)));
}

NAN_METHOD(TypeWrapper::GetStructTy)
{
    if (info.Length() == 0)
    {
        return Nan::ThrowError("Struct type expects a list of element types");
    }

    if (!info[0]->IsArray())
    {
        return Nan::ThrowError("Struct type expects a list of element types");
    }

    Local<Array> types = info[0].As<Array>();

    std::vector<std::shared_ptr<const TypeHandle>> elementTypes;

    unsigned e = types->Length();
    elementTypes.reserve(e);

    Local<FunctionTemplate> proto = Nan::New(prototype);

    for (unsigned i = 0; i < e; i += 1)
    {
        Local<Value> type = types->Get(i);
        if (!proto->HasInstance(type))
        {
            return Nan::ThrowError("Struct type expects a list of element types");
        }
        TypeWrapper *el = Nan::ObjectWrap::Unwrap<TypeWrapper>(type.As<Object>());
        elementTypes.push_back(el->Type);
    }

    info.GetReturnValue().Set(wrapType(std::make_shared<StructTypeHandle>(elementTypes)));
}

NAN_METHOD(TypeWrapper::GetFunctionTy)
{
    EXTRACT_FUNCTION_PARAMS(0)

    info.GetReturnValue().Set(wrapType(std::make_shared<FunctionTypeHandle>(returns, takes)));
}

Nan::Persistent<FunctionTemplate> TypeWrapper::prototype;
