// builder

#include "builder_wrapper.h"

#include "type_wrapper.h"
#include "value_wrapper.h"
#include "function_builder_wrapper.h"

NAN_METHOD(BuilderWrapper::New)
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

NAN_METHOD(BuilderWrapper::Alloca)
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

NAN_METHOD(BuilderWrapper::Load)
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

NAN_METHOD(BuilderWrapper::Store)
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

#define BINARY_METHOD(name) NAN_METHOD(BuilderWrapper::name)                          \
{                                                                                     \
    BuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());   \
                                                                                      \
    if (info.Length() < 2)                                                            \
    {                                                                                 \
        return Nan::ThrowError("Two values required");                                \
    }                                                                                 \
                                                                                      \
    Local<FunctionTemplate> val = Nan::New(ValueWrapper::prototype);                  \
                                                                                      \
    if (!val->HasInstance(info[0]) || !val->HasInstance(info[1]))                     \
    {                                                                                 \
        return Nan::ThrowError("Two values required");                                \
    }                                                                                 \
                                                                                      \
    ValueWrapper *lhs = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());  \
    ValueWrapper *rhs = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[1].As<Object>());  \
                                                                                      \
    ValueHandle *result = wrapper->Builder->name(lhs->Val, rhs->Val);                 \
                                                                                      \
    info.GetReturnValue().Set(ValueWrapper::wrapValue(result));                       \
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

NAN_METHOD(BuilderWrapper::Select)
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

NAN_METHOD(BuilderWrapper::Value)
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

NAN_METHOD(BuilderWrapper::Return)
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

NAN_METHOD(BuilderWrapper::Parameter)
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

NAN_METHOD(BuilderWrapper::LoadConstant)
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

NAN_METHOD(BuilderWrapper::CallFunction)
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
    else if (Nan::New(FunctionBuilderWrapper::prototype)->HasInstance(handle))
    {
        FunctionBuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<FunctionBuilderWrapper>(handle);
        FunctionBuilder *callee = wrapper->getFunctionBuilder();

        result = self->Builder->CallFunction(callee, argVals);
    }
    else
    {
        return Nan::ThrowError("Must provide function value");
    }

    info.GetReturnValue().Set(ValueWrapper::wrapValue(result));
}

NAN_METHOD(BuilderWrapper::Br)
{
    BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

    if (info.Length() == 0)
    {
        return Nan::ThrowError("Br requires a branch target");
    }

    auto builderProto = Nan::New(BuilderWrapper::prototype);
    auto valueProto = Nan::New(ValueWrapper::prototype);

    if (builderProto->HasInstance(info[0]))
    {
        // unconditional branch

        BuilderWrapper *target = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info[0].As<Object>());

        self->Builder->Br(target->Builder);
    }
    else if (valueProto->HasInstance(info[0]))
    {
        // conditional branch

        ValueWrapper *cond = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());

        if (!builderProto->HasInstance(info[1]))
        {
            return Nan::ThrowError("Br requires if true target");
        }
        BuilderWrapper *ifTrue = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info[1].As<Object>());

        if (!builderProto->HasInstance(info[2]))
        {
            return Nan::ThrowError("Br requires if false target");
        }
        BuilderWrapper *ifFalse = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info[2].As<Object>());

        self->Builder->CondBr(cond->Val, ifTrue->Builder, ifFalse->Builder);
    }
    else
    {
        return Nan::ThrowError("Br requires a branch target");
    }
}

NAN_METHOD(BuilderWrapper::If)
{
    Nan::EscapableHandleScope scope;

    BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

    if (info.Length() == 0)
    {
        return Nan::ThrowError("If requires a condition value");
    }

    if (!Nan::New(ValueWrapper::prototype)->HasInstance(info[0]))
    {
        return Nan::ThrowError("If condition must be a value");
    }

    ValueWrapper *cond = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());

    IfBuilder clauses = self->Builder->If(cond->Val);

    Local<Object> result = Nan::New<v8::Object>();

    result->Set(Nan::New("then").ToLocalChecked(), BuilderWrapper::wrapBuilder(clauses.Then));
    result->Set(Nan::New("else").ToLocalChecked(), BuilderWrapper::wrapBuilder(clauses.Else));

    info.GetReturnValue().Set(scope.Escape(result));
}

Handle<v8::Value> BuilderWrapper::wrapBuilder(InstructionBuilder *value)
{
    Nan::EscapableHandleScope scope;

    const unsigned argc = 1;
    Handle<v8::Value> argv[argc] = { Nan::New<External>((void *)value) };
    Local<Function> cons = Nan::New(constructor());

    return scope.Escape(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
}

NAN_MODULE_INIT(BuilderWrapper::Init)
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

    Nan::SetPrototypeMethod(tmpl, "br", Br);

    Nan::SetPrototypeMethod(tmpl, "if", If);

    constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
    Nan::Set(target, Nan::New("Builder").ToLocalChecked(),
        Nan::GetFunction(tmpl).ToLocalChecked());

    prototype.Reset(tmpl);
}

Nan::Persistent<FunctionTemplate> BuilderWrapper::prototype;
