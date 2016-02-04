// function builder

#include "function_builder.h"

ValueHandle *FunctionBuilder::MakeValue(TypeHandle *t, int i)
{
    return builder->MakeValue(t, i);
}

BlockBuilder *FunctionBuilder::ChildBlock(const char *name)
{
    return builder->ChildBlock(name);
}

IfBuilder FunctionBuilder::If(ValueHandle *condition)
{
    return builder->If(condition);
}

void FunctionBuilder::Br(InstructionBuilder *dest)
{
    builder->Br(dest);
}
void FunctionBuilder::CondBr(ValueHandle *condition, InstructionBuilder *ifTrue, InstructionBuilder *ifFalse)
{
    builder->CondBr(condition, ifTrue, ifFalse);
}

void FunctionBuilder::Return()
{
    builder->Return();
}

void FunctionBuilder::Return(int value)
{
    builder->Return(value);
}

void FunctionBuilder::Return(ValueHandle *value)
{
    builder->Return(value);
}

ValueHandle *FunctionBuilder::Alloca(TypeHandle *t)
{
    return builder->Alloca(t);
}

ValueHandle *FunctionBuilder::Alloca(TypeHandle *t, int size)
{
    return builder->Alloca(t, size);
}

ValueHandle *FunctionBuilder::Alloca(TypeHandle *t, ValueHandle *size)
{
    return builder->Alloca(t, size);
}

ValueHandle *FunctionBuilder::Load(ValueHandle *ptr)
{
    return builder->Load(ptr);
}

void FunctionBuilder::Store(int value, ValueHandle *ptr)
{
    builder->Store(value, ptr);
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
BINARY_BUILDER(URem)
BINARY_BUILDER(SRem)
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
