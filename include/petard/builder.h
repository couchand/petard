// abstract builder

#ifndef BUILDER_H
#define BUILDER_H

#include <vector>

#include "type.h"
#include "value.h"

class BlockBuilder;
class FunctionBuilder;
class SwitchBuilder;

class InstructionBuilder
{
public:
    virtual FunctionBuilder *GetParent() = 0;
    virtual llvm::BasicBlock *GetBlock() = 0;
    virtual void InsertAfter() = 0;
    virtual void InsertBefore() = 0;

    virtual ValueHandle *MakeValue(std::shared_ptr<const TypeHandle> t, double i) = 0;
    virtual ValueHandle *MakeUndefined(std::shared_ptr<const TypeHandle> t) = 0;

    virtual BlockBuilder *ChildBlock(const char *name) = 0;
    virtual BlockBuilder *SplitBlock(const char *name) = 0;
    virtual void UseBlock(InstructionBuilder *replacement) = 0;
    virtual void RemoveTerminator() = 0;

    virtual void Br(InstructionBuilder *dest) = 0;
    virtual void CondBr(ValueHandle *condition, InstructionBuilder *ifTrue, InstructionBuilder *ifFalse) = 0;
    virtual SwitchBuilder *Switch(ValueHandle *condition, InstructionBuilder *defaultDest) = 0;

    virtual ValueHandle *LoadConstant(ValueHandle *value) = 0;
    virtual ValueHandle *GetElementPointer(ValueHandle *ptr, std::vector<ValueHandle *> idxs) = 0;
    virtual ValueHandle *ExtractElement(ValueHandle *vec, ValueHandle *idx) = 0;
    virtual ValueHandle *InsertElement(ValueHandle *vec, ValueHandle *val, ValueHandle *idx) = 0;

    virtual ValueHandle *CallFunction(ValueHandle *fn, std::vector<ValueHandle *> args) = 0;
    virtual ValueHandle *CallFunction(FunctionBuilder *fn, std::vector<ValueHandle *> args) = 0;

    virtual void Return() = 0;
    virtual void Return(ValueHandle *value) = 0;

    virtual ValueHandle *Alloca(std::shared_ptr<const TypeHandle> type) = 0;
    virtual ValueHandle *Alloca(std::shared_ptr<const TypeHandle> type, ValueHandle *arraySize) = 0;

    virtual ValueHandle *Load(ValueHandle *ptr) = 0;

    virtual void Store(ValueHandle *value, ValueHandle *ptr) = 0;

#define BINARY_INTERFACE(name) virtual ValueHandle *name(ValueHandle *lhs, ValueHandle *rhs) = 0;

    BINARY_INTERFACE(Add)
    BINARY_INTERFACE(Sub)
    BINARY_INTERFACE(Mul)
    BINARY_INTERFACE(UDiv)
    BINARY_INTERFACE(SDiv)
    BINARY_INTERFACE(FDiv)
    BINARY_INTERFACE(URem)
    BINARY_INTERFACE(SRem)
    BINARY_INTERFACE(FRem)
    BINARY_INTERFACE(And)
    BINARY_INTERFACE(Or)
    BINARY_INTERFACE(Xor)
    BINARY_INTERFACE(Shl)
    BINARY_INTERFACE(LShr)
    BINARY_INTERFACE(AShr)

    BINARY_INTERFACE(Equal)
    BINARY_INTERFACE(NotEqual)
    BINARY_INTERFACE(UGreaterThan)
    BINARY_INTERFACE(UAtLeast)
    BINARY_INTERFACE(ULessThan)
    BINARY_INTERFACE(UAtMost)
    BINARY_INTERFACE(SGreaterThan)
    BINARY_INTERFACE(SAtLeast)
    BINARY_INTERFACE(SLessThan)
    BINARY_INTERFACE(SAtMost)

    BINARY_INTERFACE(FOEqual)
    BINARY_INTERFACE(FONotEqual)
    BINARY_INTERFACE(FOGreaterThan)
    BINARY_INTERFACE(FOAtLeast)
    BINARY_INTERFACE(FOLessThan)
    BINARY_INTERFACE(FOAtMost)
    BINARY_INTERFACE(FUEqual)
    BINARY_INTERFACE(FUNotEqual)
    BINARY_INTERFACE(FUGreaterThan)
    BINARY_INTERFACE(FUAtLeast)
    BINARY_INTERFACE(FULessThan)
    BINARY_INTERFACE(FUAtMost)

#define CAST_INTERFACE(name) virtual ValueHandle *name(ValueHandle *value, std::shared_ptr<const TypeHandle> type) = 0;

    CAST_INTERFACE(Trunc)
    CAST_INTERFACE(ZExt)
    CAST_INTERFACE(SExt)
    CAST_INTERFACE(FPToUI)
    CAST_INTERFACE(FPToSI)
    CAST_INTERFACE(UIToFP)
    CAST_INTERFACE(SIToFP)
    CAST_INTERFACE(FPTrunc)
    CAST_INTERFACE(FPExt)
    CAST_INTERFACE(PtrToInt)
    CAST_INTERFACE(IntToPtr)
    CAST_INTERFACE(Bitcast)

    virtual ValueHandle *Select(ValueHandle *cond, ValueHandle *ifTrue, ValueHandle *ifFalse) = 0;

    virtual ValueHandle *Parameter(size_t index) = 0;
};

#endif
