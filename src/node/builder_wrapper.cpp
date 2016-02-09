// builder

#include "builder_wrapper.h"

#include "type_wrapper.h"
#include "value_wrapper.h"
#include "function_builder_wrapper.h"
#include "switch_builder_wrapper.h"

#include "nan_macros.h"

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

    EXPECT_PARAM("Alloca", 0, TypeWrapper, "type")
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

        ValueHandle *arraySize = wrapper->Builder->MakeValue(new IntTypeHandle(32), numVal);
        h = wrapper->Builder->Alloca(t->Type, arraySize);
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
    
    EXPECT_PARAM("Load", 0, ValueWrapper, "pointer to load from")
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

    EXPECT_PARAM("Store", 1, ValueWrapper, "pointer to store to")
    ValueWrapper *ptr = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[1].As<Object>());

    if (!ptr->Val->Type->isPointerType())
    {
        return Nan::ThrowError("Store requires pointer type");
    }

    PointerTypeHandle *pt = static_cast<PointerTypeHandle *>(ptr->Val->Type);

    if (info[0]->IsNumber())
    {
        if (!pt->pointee->isIntType() && !pt->pointee->isFloatType())
        {
            return Nan::ThrowError("Store type mismatch");
        }

        Local<Number> num = info[0].As<Number>();
        double numVal = num->Value();

        ValueHandle *storageVal = wrapper->Builder->MakeValue(pt->pointee, numVal);

        wrapper->Builder->Store(storageVal, ptr->Val);
    }
    else if (Nan::New(ValueWrapper::prototype)->HasInstance(info[0]))
    {
        ValueWrapper *value = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());

        if (!value->Val->Type->isCompatibleWith(pt->pointee))
        {
            return Nan::ThrowError("Store type mismatch");
        }

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
BINARY_METHOD(FDiv)
BINARY_METHOD(URem)
BINARY_METHOD(SRem)
BINARY_METHOD(FRem)
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
BINARY_METHOD(FOEqual)
BINARY_METHOD(FONotEqual)
BINARY_METHOD(FOGreaterThan)
BINARY_METHOD(FOAtLeast)
BINARY_METHOD(FOLessThan)
BINARY_METHOD(FOAtMost)
BINARY_METHOD(FUEqual)
BINARY_METHOD(FUNotEqual)
BINARY_METHOD(FUGreaterThan)
BINARY_METHOD(FUAtLeast)
BINARY_METHOD(FULessThan)
BINARY_METHOD(FUAtMost)

#define CAST_METHOD(name) NAN_METHOD(BuilderWrapper::name)                            \
{                                                                                     \
    BuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());   \
                                                                                      \
    if (info.Length() < 2)                                                            \
    {                                                                                 \
        return Nan::ThrowError("A value and a type required");                        \
    }                                                                                 \
                                                                                      \
    Local<FunctionTemplate> val = Nan::New(ValueWrapper::prototype);                  \
    Local<FunctionTemplate> typ = Nan::New(TypeWrapper::prototype);                   \
                                                                                      \
    if (!val->HasInstance(info[0]) || !typ->HasInstance(info[1]))                     \
    {                                                                                 \
        return Nan::ThrowError("A value and a type required");                        \
    }                                                                                 \
                                                                                      \
    ValueWrapper *v = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());    \
    TypeWrapper *t = Nan::ObjectWrap::Unwrap<TypeWrapper>(info[1].As<Object>());      \
                                                                                      \
    ValueHandle *result = wrapper->Builder->name(v->Val, t->Type);                    \
                                                                                      \
    info.GetReturnValue().Set(ValueWrapper::wrapValue(result));                       \
}

CAST_METHOD(Trunc)
CAST_METHOD(ZExt)
CAST_METHOD(SExt)
CAST_METHOD(FPToUI)
CAST_METHOD(FPToSI)
CAST_METHOD(UIToFP)
CAST_METHOD(SIToFP)
CAST_METHOD(FPTrunc)
CAST_METHOD(FPExt)
CAST_METHOD(PtrToInt)
CAST_METHOD(IntToPtr)
CAST_METHOD(Bitcast)

NAN_METHOD(BuilderWrapper::Select)
{
    BuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

    EXPECT_PARAM("Select", 0, ValueWrapper, "condition")
    ValueWrapper *cond = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());

    if (!cond->Val->Type->isCompatibleWith(new IntTypeHandle(1)))
    {
        return Nan::ThrowError("Select condition must be an i1"); // or a vector...
    }

    EXPECT_PARAM("Select", 1, ValueWrapper, "ifTrue")
    ValueWrapper *ifTrue = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[1].As<Object>());

    EXPECT_PARAM("Select", 2, ValueWrapper, "ifFalse")
    ValueWrapper *ifFalse = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[2].As<Object>());

    if (!ifTrue->Val->Type->isCompatibleWith(ifFalse->Val->Type))
    {
        return Nan::ThrowError("Select values must be the same type");
    }

    ValueHandle *result = wrapper->Builder->Select(cond->Val, ifTrue->Val, ifFalse->Val);

    info.GetReturnValue().Set(ValueWrapper::wrapValue(result));
}

NAN_METHOD(BuilderWrapper::Value)
{
    BuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

    EXPECT_PARAM("Value", 0, TypeWrapper, "type")
    TypeWrapper *type = Nan::ObjectWrap::Unwrap<TypeWrapper>(info[0].As<Object>());

    if (info.Length() < 2)
    {
        return Nan::ThrowError("Value requires a constant value");
    }

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

    FunctionTypeHandle *parentType = wrapper->Builder->GetParent()->Type;
    TypeHandle *returnType = parentType->returns;

    if (returnType->isVoidType())
    {
        if (info.Length() == 0)
        {
            wrapper->Builder->Return();
        }
        else
        {
            return Nan::ThrowError("Invalid return type from void function");
        }
    }
    else if (info[0]->IsNumber())
    {
        if (!returnType->isIntType() && !returnType->isFloatType())
        {
            return Nan::ThrowError("Function return type is not numeric");
        }

        Local<Number> num = info[0].As<Number>();
        double numVal = num->Value();

        ValueHandle *returnVal = wrapper->Builder->MakeValue(returnType, numVal);
        wrapper->Builder->Return(returnVal);
    }
    else if (Nan::New(ValueWrapper::prototype)->HasInstance(info[0]))
    {
        ValueWrapper *value = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());

        if (!value->Val->Type->isCompatibleWith(returnType))
        {
            return Nan::ThrowError("Return type mismatch");
        }

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

    EXPECT_PARAM("LoadConstant", 0, ValueWrapper, "constant value")
    ValueWrapper *wrapper = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0]->ToObject());

    ValueHandle *result = self->Builder->LoadConstant(wrapper->Val);

    if (!result)
    {
        return Nan::ThrowError("Load constant error");
    }

    info.GetReturnValue().Set(ValueWrapper::wrapValue(result));
}

ValueHandle *getValueHandle(InstructionBuilder *builder, Local<v8::Value> thing)
{
    if (thing->IsNumber())
    {
        Local<Number> num = thing.As<Number>();
        double val = num->Value();
        return builder->MakeValue(new IntTypeHandle(32), val);
    }
    if (Nan::New(ValueWrapper::prototype)->HasInstance(thing))
    {
        ValueWrapper *wrapper = Nan::ObjectWrap::Unwrap<ValueWrapper>(thing.As<Object>());
        return wrapper->Val;
    }
    return 0;
}

NAN_METHOD(BuilderWrapper::GetElementPointer)
{
    BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

    EXPECT_PARAM("GetElementPointer", 0, ValueWrapper, "pointer base")

    ValueWrapper *base = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());

    if (info.Length() == 1)
    {
        return Nan::ThrowError("GetElementPointer expects at least one index");
    }

    std::vector<ValueHandle *> idxs;

    for (unsigned i = 1, e = info.Length(); i < e; i += 1)
    {
        ValueHandle *next = getValueHandle(self->Builder, info[i]);

        if (!next)
        {
            return Nan::ThrowError("GetElementPointer expects index values to be values");
        }

        idxs.push_back(next);
    }

    ValueHandle *result = self->Builder->GetElementPointer(base->Val, idxs);

    if (!result)
    {
        return Nan::ThrowError("GetElementPointer index error");
    }

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
    std::vector<TypeHandle *> argTypes;

    for (unsigned i = 1, e = info.Length(); i < e; i += 1)
    {
        EXPECT_PARAM("CallFunction", i, ValueWrapper, "argument value")
        ValueWrapper *arg = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[i]->ToObject());
        argVals.push_back(arg->Val);
        argTypes.push_back(arg->Val->Type);
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

        FunctionTypeHandle *fnty = static_cast<FunctionTypeHandle *>(callee->Type);

        if (fnty->params.size() != argTypes.size())
        {
            return Nan::ThrowError("Incorrect number of parameters for function call");
        }

        for (unsigned i = 0, e = argTypes.size(); i < e; i += 1)
        {
            if (!argTypes[i]->isCompatibleWith(fnty->params[i]))
            {
                return Nan::ThrowError("Type mismatch in parameters for function call");
            }
        }

        result = self->Builder->CallFunction(callee, argVals);
    }
    else if (Nan::New(FunctionBuilderWrapper::prototype)->HasInstance(handle))
    {
        FunctionBuilderWrapper *wrapper = Nan::ObjectWrap::Unwrap<FunctionBuilderWrapper>(handle);
        FunctionBuilder *callee = wrapper->getFunctionBuilder();

        FunctionTypeHandle *fnty = static_cast<FunctionTypeHandle *>(callee->Type);

        if (fnty->params.size() != argTypes.size())
        {
            return Nan::ThrowError("Incorrect number of parameters for function call");
        }

        for (unsigned i = 0, e = argTypes.size(); i < e; i += 1)
        {
            if (!argTypes[i]->isCompatibleWith(fnty->params[i]))
            {
                return Nan::ThrowError("Type mismatch in parameters for function call");
            }
        }

        result = self->Builder->CallFunction(callee, argVals);
    }
    else
    {
        return Nan::ThrowError("Must provide function value");
    }

    info.GetReturnValue().Set(ValueWrapper::wrapValue(result));
}

NAN_METHOD(BuilderWrapper::Switch)
{
    BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

    EXPECT_PARAM("Switch", 0, ValueWrapper, "condition")
    ValueWrapper *cond = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0].As<Object>());

    EXPECT_PARAM("Switch", 1, BuilderWrapper, "default target")
    BuilderWrapper *def = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info[1].As<Object>());

    SwitchBuilder *sw = self->Builder->Switch(cond->Val, def->Builder);
    info.GetReturnValue().Set(SwitchBuilderWrapper::wrapSwitchBuilder(sw));
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

NAN_METHOD(BuilderWrapper::CreateBlock)
{
    BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

    if (info.Length() == 0)
    {
        // TODO: should we really?
        return Nan::ThrowError("CreateBlock requires a block name");
    }

    if (!info[0]->IsString())
    {
        return Nan::ThrowError("Block name must be a string");
    }

    Local<String> name = info[0].As<String>();
    String::Utf8Value encoded(name);

    InstructionBuilder *child = self->Builder->ChildBlock(*encoded);
    info.GetReturnValue().Set(BuilderWrapper::wrapBuilder(child));
}

NAN_METHOD(BuilderWrapper::SplitBlock)
{
    BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

    if (info.Length() == 0)
    {
        // TODO: should we really?
        return Nan::ThrowError("SplitBlock requires a block name");
    }

    if (!info[0]->IsString())
    {
        return Nan::ThrowError("Block name must be a string");
    }

    Local<String> name = info[0].As<String>();
    String::Utf8Value encoded(name);

    InstructionBuilder *child = self->Builder->SplitBlock(*encoded);
    info.GetReturnValue().Set(BuilderWrapper::wrapBuilder(child));
}

NAN_METHOD(BuilderWrapper::UseBlock)
{
    BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

    if (!Nan::New(BuilderWrapper::prototype)->HasInstance(info[0]))
    {
        return Nan::ThrowError("UseBlock requires a builder");
    }

    BuilderWrapper *replacement = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info[0].As<Object>());

    self->Builder->UseBlock(replacement->Builder);
}

NAN_METHOD(BuilderWrapper::InsertBefore)
{
    BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());
    self->Builder->InsertBefore();
}

NAN_METHOD(BuilderWrapper::InsertAfter)
{
    BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());
    self->Builder->InsertAfter();
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
    Nan::SetPrototypeMethod(tmpl, "getElementPointer", GetElementPointer);
    Nan::SetPrototypeMethod(tmpl, "callFunction", CallFunction);
    Nan::SetPrototypeMethod(tmpl, "alloca", Alloca);
    Nan::SetPrototypeMethod(tmpl, "load", Load);
    Nan::SetPrototypeMethod(tmpl, "store", Store);
    Nan::SetPrototypeMethod(tmpl, "add", Add);
    Nan::SetPrototypeMethod(tmpl, "sub", Sub);
    Nan::SetPrototypeMethod(tmpl, "mul", Mul);
    Nan::SetPrototypeMethod(tmpl, "udiv", UDiv);
    Nan::SetPrototypeMethod(tmpl, "sdiv", SDiv);
    Nan::SetPrototypeMethod(tmpl, "fdiv", FDiv);
    Nan::SetPrototypeMethod(tmpl, "urem", URem);
    Nan::SetPrototypeMethod(tmpl, "srem", SRem);
    Nan::SetPrototypeMethod(tmpl, "frem", FRem);
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

    Nan::SetPrototypeMethod(tmpl, "foEqual", FOEqual);
    Nan::SetPrototypeMethod(tmpl, "foNotEqual", FONotEqual);
    Nan::SetPrototypeMethod(tmpl, "foGreaterThan", FOGreaterThan);
    Nan::SetPrototypeMethod(tmpl, "foAtLeast", FOAtLeast);
    Nan::SetPrototypeMethod(tmpl, "foLessThan", FOLessThan);
    Nan::SetPrototypeMethod(tmpl, "foAtMost", FOAtMost);
    Nan::SetPrototypeMethod(tmpl, "fuEqual", FUEqual);
    Nan::SetPrototypeMethod(tmpl, "fuNotEqual", FUNotEqual);
    Nan::SetPrototypeMethod(tmpl, "fuGreaterThan", FUGreaterThan);
    Nan::SetPrototypeMethod(tmpl, "fuAtLeast", FUAtLeast);
    Nan::SetPrototypeMethod(tmpl, "fuLessThan", FULessThan);
    Nan::SetPrototypeMethod(tmpl, "fuAtMost", FUAtMost);

    Nan::SetPrototypeMethod(tmpl, "trunc", Trunc);
    Nan::SetPrototypeMethod(tmpl, "zext", ZExt);
    Nan::SetPrototypeMethod(tmpl, "sext", SExt);
    Nan::SetPrototypeMethod(tmpl, "fpToUI", FPToUI);
    Nan::SetPrototypeMethod(tmpl, "fpToSI", FPToSI);
    Nan::SetPrototypeMethod(tmpl, "uiToFP", UIToFP);
    Nan::SetPrototypeMethod(tmpl, "siToFP", SIToFP);
    Nan::SetPrototypeMethod(tmpl, "fpTrunc", FPTrunc);
    Nan::SetPrototypeMethod(tmpl, "fpext", FPExt);
    Nan::SetPrototypeMethod(tmpl, "ptrToInt", PtrToInt);
    Nan::SetPrototypeMethod(tmpl, "intToPtr", IntToPtr);
    Nan::SetPrototypeMethod(tmpl, "bitcast", Bitcast);

    Nan::SetPrototypeMethod(tmpl, "select", Select);

    Nan::SetPrototypeMethod(tmpl, "value", Value);

    Nan::SetPrototypeMethod(tmpl, "br", Br);
    Nan::SetPrototypeMethod(tmpl, "switch", Switch);

    Nan::SetPrototypeMethod(tmpl, "createBlock", CreateBlock);
    Nan::SetPrototypeMethod(tmpl, "splitBlock", SplitBlock);
    Nan::SetPrototypeMethod(tmpl, "useBlock", UseBlock);
    Nan::SetPrototypeMethod(tmpl, "insertAfter", InsertAfter);
    Nan::SetPrototypeMethod(tmpl, "insertBefore", InsertBefore);

    constructor().Reset(Nan::GetFunction(tmpl).ToLocalChecked());
    Nan::Set(target, Nan::New("Builder").ToLocalChecked(),
        Nan::GetFunction(tmpl).ToLocalChecked());

    prototype.Reset(tmpl);
}

Nan::Persistent<FunctionTemplate> BuilderWrapper::prototype;
