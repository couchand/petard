// function builder

#include "function_builder.h"

std::shared_ptr<ValueHandle> FunctionBuilder::MakeValue(std::shared_ptr<const TypeHandle> t, double i)
{
    return builder->MakeValue(std::move(t), i);
}

std::shared_ptr<ValueHandle> FunctionBuilder::MakeUndefined(std::shared_ptr<const TypeHandle> t)
{
    return builder->MakeUndefined(std::move(t));
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
void FunctionBuilder::CondBr(std::shared_ptr<ValueHandle> condition, InstructionBuilder *ifTrue, InstructionBuilder *ifFalse)
{
    builder->CondBr(condition, ifTrue, ifFalse);
}

SwitchBuilder *FunctionBuilder::Switch(std::shared_ptr<ValueHandle> condition, InstructionBuilder *defaultDest)
{
    return builder->Switch(condition, defaultDest);
}

void FunctionBuilder::Return()
{
    builder->Return();
}

void FunctionBuilder::Return(std::shared_ptr<ValueHandle> value)
{
    builder->Return(value);
}

std::shared_ptr<ValueHandle> FunctionBuilder::Alloca(std::shared_ptr<const TypeHandle> t)
{
    return builder->Alloca(std::move(t));
}

std::shared_ptr<ValueHandle> FunctionBuilder::Alloca(std::shared_ptr<const TypeHandle> t, std::shared_ptr<ValueHandle> size)
{
    return builder->Alloca(std::move(t), size);
}

std::shared_ptr<ValueHandle> FunctionBuilder::Load(std::shared_ptr<ValueHandle> ptr)
{
    return builder->Load(ptr);
}

void FunctionBuilder::Store(std::shared_ptr<ValueHandle> value, std::shared_ptr<ValueHandle> ptr)
{
    builder->Store(value, ptr);
}

#define BINARY_BUILDER(name) \
std::shared_ptr<ValueHandle> FunctionBuilder::name(std::shared_ptr<ValueHandle> lhs, std::shared_ptr<ValueHandle> rhs) \
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

#define CAST_BUILDER(name) std::shared_ptr<ValueHandle> FunctionBuilder::name(std::shared_ptr<ValueHandle> value, std::shared_ptr<const TypeHandle> type) \
{ \
    return builder->name(value, std::move(type)); \
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
CAST_BUILDER(PtrToInt)
CAST_BUILDER(IntToPtr)
CAST_BUILDER(Bitcast)

std::shared_ptr<ValueHandle> FunctionBuilder::Select(std::shared_ptr<ValueHandle> cond, std::shared_ptr<ValueHandle> ifTrue, std::shared_ptr<ValueHandle> ifFalse)
{
    return builder->Select(cond, ifTrue, ifFalse);
}

std::shared_ptr<ValueHandle> FunctionBuilder::Parameter(size_t index)
{
    if (index >= Type->params.size())
    {
        return 0;
    }
    std::shared_ptr<const TypeHandle> t = Type->params[index];
    return std::make_shared<PlainValueHandle>(std::move(t), parameters[index]);
}

std::shared_ptr<ValueHandle> FunctionBuilder::LoadConstant(std::shared_ptr<ValueHandle> value)
{
    return builder->LoadConstant(value);
}

std::shared_ptr<ValueHandle> FunctionBuilder::GetElementPointer(std::shared_ptr<ValueHandle> ptr, std::vector<std::shared_ptr<ValueHandle>> idxs)
{
    return builder->GetElementPointer(ptr, idxs);
}

std::shared_ptr<ValueHandle> FunctionBuilder::ExtractElement(std::shared_ptr<ValueHandle> vec, std::shared_ptr<ValueHandle> idx)
{
    return builder->ExtractElement(vec, idx);
}

std::shared_ptr<ValueHandle> FunctionBuilder::InsertElement(std::shared_ptr<ValueHandle> vec, std::shared_ptr<ValueHandle> val, std::shared_ptr<ValueHandle> idx)
{
    return builder->InsertElement(vec, val, idx);
}

std::shared_ptr<ValueHandle> FunctionBuilder::CallFunction(std::shared_ptr<ValueHandle> fn, std::vector<std::shared_ptr<ValueHandle>> args)
{
    return builder->CallFunction(fn, args);
}

std::shared_ptr<ValueHandle> FunctionBuilder::CallFunction(FunctionBuilder *fn, std::vector<std::shared_ptr<ValueHandle>> args)
{
    return builder->CallFunction(fn, args);
}
