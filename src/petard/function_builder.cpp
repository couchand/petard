// function builder

#include "function_builder.h"

ValueHandle *FunctionBuilder::MakeValue(TypeHandle *t, double i)
{
    return builder->MakeValue(t, i);
}

BlockBuilder *FunctionBuilder::ChildBlock(const char *name)
{
    return builder->ChildBlock(name);
}

BlockBuilder *FunctionBuilder::SplitBlock(const char *name)
{
    return builder->SplitBlock(name);
}

void FunctionBuilder::UseBlock(InstructionBuilder *replacement)
{
    builder->UseBlock(replacement);
}

void FunctionBuilder::RemoveTerminator()
{
    builder->RemoveTerminator();
}

void FunctionBuilder::Br(InstructionBuilder *dest)
{
    builder->Br(dest);
}
void FunctionBuilder::CondBr(ValueHandle *condition, InstructionBuilder *ifTrue, InstructionBuilder *ifFalse)
{
    builder->CondBr(condition, ifTrue, ifFalse);
}

SwitchBuilder *FunctionBuilder::Switch(ValueHandle *condition, InstructionBuilder *defaultDest)
{
    return builder->Switch(condition, defaultDest);
}

void FunctionBuilder::Return()
{
    builder->Return();
}

void FunctionBuilder::Return(ValueHandle *value)
{
    builder->Return(value);
}

ValueHandle *FunctionBuilder::Alloca(TypeHandle *t)
{
    return builder->Alloca(t);
}

ValueHandle *FunctionBuilder::Alloca(TypeHandle *t, ValueHandle *size)
{
    return builder->Alloca(t, size);
}

ValueHandle *FunctionBuilder::Load(ValueHandle *ptr)
{
    return builder->Load(ptr);
}

void FunctionBuilder::Store(ValueHandle *value, ValueHandle *ptr)
{
    builder->Store(value, ptr);
}

#define BINARY_BUILDER(name) \
ValueHandle *FunctionBuilder::name(ValueHandle *lhs, ValueHandle *rhs) \
{ \
    return builder->name(lhs, rhs); \
}

BINARY_BUILDER(Add)
BINARY_BUILDER(Sub)
BINARY_BUILDER(Mul)
BINARY_BUILDER(UDiv)
BINARY_BUILDER(SDiv)
BINARY_BUILDER(FDiv)
BINARY_BUILDER(URem)
BINARY_BUILDER(SRem)
BINARY_BUILDER(FRem)
BINARY_BUILDER(And)
BINARY_BUILDER(Or)
BINARY_BUILDER(Xor)
BINARY_BUILDER(Shl)
BINARY_BUILDER(LShr)
BINARY_BUILDER(AShr)
BINARY_BUILDER(Equal)
BINARY_BUILDER(NotEqual)
BINARY_BUILDER(UGreaterThan)
BINARY_BUILDER(UAtLeast)
BINARY_BUILDER(ULessThan)
BINARY_BUILDER(UAtMost)
BINARY_BUILDER(SGreaterThan)
BINARY_BUILDER(SAtLeast)
BINARY_BUILDER(SLessThan)
BINARY_BUILDER(SAtMost)

BINARY_BUILDER(FOEqual)
BINARY_BUILDER(FONotEqual)
BINARY_BUILDER(FOGreaterThan)
BINARY_BUILDER(FOAtLeast)
BINARY_BUILDER(FOLessThan)
BINARY_BUILDER(FOAtMost)
BINARY_BUILDER(FUEqual)
BINARY_BUILDER(FUNotEqual)
BINARY_BUILDER(FUGreaterThan)
BINARY_BUILDER(FUAtLeast)
BINARY_BUILDER(FULessThan)
BINARY_BUILDER(FUAtMost)

#define CAST_BUILDER(name) ValueHandle *FunctionBuilder::name(ValueHandle *value, TypeHandle *type) \
{ \
    return builder->name(value, type); \
}

CAST_BUILDER(Trunc)
CAST_BUILDER(ZExt)
CAST_BUILDER(SExt)
CAST_BUILDER(FPToUI)
CAST_BUILDER(FPToSI)
CAST_BUILDER(UIToFP)
CAST_BUILDER(SIToFP)
CAST_BUILDER(FPTrunc)
CAST_BUILDER(FPExt)

ValueHandle *FunctionBuilder::Select(ValueHandle *cond, ValueHandle *ifTrue, ValueHandle *ifFalse)
{
    return builder->Select(cond, ifTrue, ifFalse);
}

ValueHandle *FunctionBuilder::Parameter(size_t index)
{
    if (index >= Type->params.size())
    {
        return 0;
    }
    TypeHandle *t = Type->params[index];
    return new PlainValueHandle(t, parameters[index]);
}

ValueHandle *FunctionBuilder::LoadConstant(ValueHandle *value)
{
    return builder->LoadConstant(value);
}

ValueHandle *FunctionBuilder::CallFunction(ValueHandle *fn, std::vector<ValueHandle *> args)
{
    return builder->CallFunction(fn, args);
}

ValueHandle *FunctionBuilder::CallFunction(FunctionBuilder *fn, std::vector<ValueHandle *> args)
{
    return builder->CallFunction(fn, args);
}
