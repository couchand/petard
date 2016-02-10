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

    if (!h)
    {
        return Nan::ThrowError("Uncaught error in Alloca!");
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

    if (!load)
    {
        return Nan::ThrowError("Uncaught error in Load!");
    }
    
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

#define BINARY_METHOD(name, pred) NAN_METHOD(BuilderWrapper::name)                    \
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
    TypeHandle *lt = lhs->Val->Type;                                                  \
    TypeHandle *rt = rhs->Val->Type;                                                  \
                                                                                      \
    TypeHandle *let = lt;                                                             \
    TypeHandle *ret = rt;                                                             \
                                                                                      \
    if (lt->isVectorType())                                                           \
    {                                                                                 \
        if (!rt->isVectorType())                                                      \
        {                                                                             \
            return Nan::ThrowError("If one value is a vector, both must be");         \
        }                                                                             \
                                                                                      \
        VectorTypeHandle *lvt = static_cast<VectorTypeHandle *>(lt);                  \
        let = lvt->element;                                                           \
        VectorTypeHandle *rvt = static_cast<VectorTypeHandle *>(rt);                  \
        ret = rvt->element;                                                           \
                                                                                      \
        if (lvt->size != rvt->size)                                                   \
        {                                                                             \
            return Nan::ThrowError("Vectors must have the same length");              \
        }                                                                             \
    }                                                                                 \
                                                                                      \
    if (!pred(let) || !pred(ret))                                                     \
    {                                                                                 \
        return Nan::ThrowError("Error with value types in binary");                   \
    }                                                                                 \
                                                                                      \
    if (!lt->isCompatibleWith(rt))                                                    \
    {                                                                                 \
        return Nan::ThrowError("Type mismatch in binary");                            \
    }                                                                                 \
                                                                                      \
                                                                                      \
    ValueHandle *result = wrapper->Builder->name(lhs->Val, rhs->Val);                 \
                                                                                      \
    if (!result)                                                                      \
    {                                                                                 \
        return Nan::ThrowError("Uncaught error in binary");                           \
    }                                                                                 \
                                                                                      \
    info.GetReturnValue().Set(ValueWrapper::wrapValue(result));                       \
}

bool isFloat(TypeHandle *t) { return t->isFloatType(); }
bool isInt(TypeHandle *t) { return t->isIntType(); }
bool isNum(TypeHandle *t) { return t->isFloatType() || t->isIntType(); }

BINARY_METHOD(Add, isNum)
BINARY_METHOD(Sub, isNum)
BINARY_METHOD(Mul, isNum)
BINARY_METHOD(UDiv, isInt)
BINARY_METHOD(SDiv, isInt)
BINARY_METHOD(FDiv, isFloat)
BINARY_METHOD(URem, isInt)
BINARY_METHOD(SRem, isInt)
BINARY_METHOD(FRem, isFloat)
BINARY_METHOD(And, isInt)
BINARY_METHOD(Or, isInt)
BINARY_METHOD(Xor, isInt)
BINARY_METHOD(Shl, isInt)
BINARY_METHOD(LShr, isInt)
BINARY_METHOD(AShr, isInt)

BINARY_METHOD(Equal, isInt)
BINARY_METHOD(NotEqual, isInt)
BINARY_METHOD(UGreaterThan, isInt)
BINARY_METHOD(UAtLeast, isInt)
BINARY_METHOD(ULessThan, isInt)
BINARY_METHOD(UAtMost, isInt)
BINARY_METHOD(SGreaterThan, isInt)
BINARY_METHOD(SAtLeast, isInt)
BINARY_METHOD(SLessThan, isInt)
BINARY_METHOD(SAtMost, isInt)
BINARY_METHOD(FOEqual, isFloat)
BINARY_METHOD(FONotEqual, isFloat)
BINARY_METHOD(FOGreaterThan, isFloat)
BINARY_METHOD(FOAtLeast, isFloat)
BINARY_METHOD(FOLessThan, isFloat)
BINARY_METHOD(FOAtMost, isFloat)
BINARY_METHOD(FUEqual, isFloat)
BINARY_METHOD(FUNotEqual, isFloat)
BINARY_METHOD(FUGreaterThan, isFloat)
BINARY_METHOD(FUAtLeast, isFloat)
BINARY_METHOD(FULessThan, isFloat)
BINARY_METHOD(FUAtMost, isFloat)

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
    if (!result)                                                                      \
    {                                                                                 \
        return Nan::ThrowError("Uncaught error in cast!");                            \
    }                                                                                 \
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

    TypeHandle *condTy = cond->Val->Type;

    if (!condTy->isVectorType() && !condTy->isCompatibleWith(new IntTypeHandle(1)))
    {
        return Nan::ThrowError("Select condition must be an i1");
    }

    EXPECT_PARAM("Select", 1, ValueWrapper, "ifTrue")
    ValueWrapper *ifTrue = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[1].As<Object>());

    EXPECT_PARAM("Select", 2, ValueWrapper, "ifFalse")
    ValueWrapper *ifFalse = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[2].As<Object>());

    if (condTy->isVectorType())
    {
        TypeHandle *tTy = ifTrue->Val->Type;
        TypeHandle *fTy = ifFalse->Val->Type;

        if (!tTy->isVectorType() || !fTy->isVectorType())
        {
            return Nan::ThrowError("Select with vector condition requires vector values");
        }

        VectorTypeHandle *cv = static_cast<VectorTypeHandle *>(condTy);
        VectorTypeHandle *tv = static_cast<VectorTypeHandle *>(tTy);
        VectorTypeHandle *fv = static_cast<VectorTypeHandle *>(fTy);
        if (cv->size != tv->size || cv->size != fv->size)
        {
            return Nan::ThrowError("Select with vectors requires them to be the same size");
        }
    }

    if (!ifTrue->Val->Type->isCompatibleWith(ifFalse->Val->Type))
    {
        return Nan::ThrowError("Select values must be the same type");
    }

    ValueHandle *result = wrapper->Builder->Select(cond->Val, ifTrue->Val, ifFalse->Val);

    if (!result)
    {
        return Nan::ThrowError("Uncaught error in Select!");
    }

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

        result = wrapper->Builder->MakeValue(type->Type, numVal);
    }
    else
    {
        return Nan::ThrowError("Value type not supported");
    }

    if (!result)
    {
        return Nan::ThrowError("Uncaught error in Value!");
    }

    info.GetReturnValue().Set(ValueWrapper::wrapValue(result));
}

NAN_METHOD(BuilderWrapper::Undefined)
{
    BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

    EXPECT_PARAM("Undefined", 0, TypeWrapper, "type")
    TypeWrapper *type = Nan::ObjectWrap::Unwrap<TypeWrapper>(info[0].As<Object>());

    ValueHandle *result = self->Builder->MakeUndefined(type->Type);

    if (!result)
    {
        return Nan::ThrowError("Uncaught error in Undefined!");
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

    if (!val)
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

NAN_METHOD(BuilderWrapper::ExtractElement)
{
    BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

    EXPECT_PARAM("ExtractElement", 0, ValueWrapper, "vector")
    ValueWrapper *value = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0]->ToObject());

    if (!value->Val->Type->isVectorType())
    {
        return Nan::ThrowError("ExtractElement vector must be a vector type");
    }

    if (info.Length() <= 1)
    {
        return Nan::ThrowError("ExtractElement requires an index");
    }

    ValueHandle *index = getValueHandle(self->Builder, info[1]);
    if (!index)
    {
        return Nan::ThrowError("ExtractElement requires an index");
    }

    if (!index->Type->isIntType())
    {
        return Nan::ThrowError("ExtractElement index must be an integer type");
    }

    ValueHandle *result = self->Builder->ExtractElement(value->Val, index);

    if (!result)
    {
        return Nan::ThrowError("Uncaught error in ExtractElement!");
    }

    info.GetReturnValue().Set(ValueWrapper::wrapValue(result));
}

NAN_METHOD(BuilderWrapper::InsertElement)
{
    BuilderWrapper *self = Nan::ObjectWrap::Unwrap<BuilderWrapper>(info.This());

    EXPECT_PARAM("InsertElement", 0, ValueWrapper, "vector")
    ValueWrapper *vec = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[0]->ToObject());

    if (!vec->Val->Type->isVectorType())
    {
        return Nan::ThrowError("InsertElement vector must be a vector type");
    }

    EXPECT_PARAM("InsertElement", 1, ValueWrapper, "value")
    ValueWrapper *value = Nan::ObjectWrap::Unwrap<ValueWrapper>(info[1]->ToObject());

    VectorTypeHandle *vecTy = static_cast<VectorTypeHandle *>(vec->Val->Type);
    if (!value->Val->Type->isCompatibleWith(vecTy->element))
    {
        return Nan::ThrowError("Incompatible types in InsertElement");
    }

    if (info.Length() <= 2)
    {
        return Nan::ThrowError("InsertElement requires an index");
    }

    ValueHandle *index = getValueHandle(self->Builder, info[2]);
    if (!index)
    {
        return Nan::ThrowError("InsertElement requires an index");
    }

    if (!index->Type->isIntType())
    {
        return Nan::ThrowError("InsertElement index must be an integer type");
    }

    ValueHandle *result = self->Builder->InsertElement(vec->Val, value->Val, index);

    if (!result)
    {
        return Nan::ThrowError("Uncaught error in InsertElement!");
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

    if (!result)
    {
        return Nan::ThrowError("Uncaught error in FunctionCall!");
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

    if (!sw)
    {
        return Nan::ThrowError("Uncaught error in Switch!");
    }

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

    if (!child)
    {
        return Nan::ThrowError("Uncaught error in CreateBlock!");
    }

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

    if (!child)
    {
        return Nan::ThrowError("Uncaught error in SplitBlock!");
    }

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
    Nan::SetPrototypeMethod(tmpl, "extractElement", ExtractElement);
    Nan::SetPrototypeMethod(tmpl, "insertElement", InsertElement);

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
    Nan::SetPrototypeMethod(tmpl, "undefined", Undefined);

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
