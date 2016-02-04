// abstract builder

#ifndef BUILDER_H
#define BUILDER_H

#include <string>
#include <vector>

#include "llvm/IR/IRBuilder.h"

#include "types.h"
#include "value.h"

class BlockBuilder;
class FunctionBuilder;

struct IfBuilder
{
    BlockBuilder *Then;
    BlockBuilder *Else;
};

class InstructionBuilder
{
public:
    virtual llvm::BasicBlock *GetBlock() = 0;
    virtual void InsertAfter() = 0;
    virtual void InsertBefore() = 0;

    virtual ValueHandle *MakeValue(TypeHandle *t, int i) = 0;

    virtual BlockBuilder *ChildBlock(const char *name) = 0;

    virtual IfBuilder If(ValueHandle *condition) = 0;
    virtual void Br(InstructionBuilder *dest) = 0;

    virtual ValueHandle *LoadConstant(ValueHandle *value) = 0;

    virtual ValueHandle *CallFunction(ValueHandle *fn, std::vector<ValueHandle *> args) = 0;
    virtual ValueHandle *CallFunction(FunctionBuilder *fn, std::vector<ValueHandle *> args) = 0;

    virtual void Return() = 0;
    virtual void Return(int value) = 0;
    virtual void Return(ValueHandle *value) = 0;

    virtual ValueHandle *Alloca(TypeHandle *type) = 0;
    virtual ValueHandle *Alloca(TypeHandle *type, int arraySize) = 0;
    virtual ValueHandle *Alloca(TypeHandle *type, ValueHandle *arraySize) = 0;

    virtual ValueHandle *Load(ValueHandle *ptr) = 0;

    virtual void Store(int value, ValueHandle *ptr) = 0;
    virtual void Store(ValueHandle *value, ValueHandle *ptr) = 0;

#define BINARY_INTERFACE(name) virtual ValueHandle *name(ValueHandle *lhs, ValueHandle *rhs) = 0;

    BINARY_INTERFACE(Add)
    BINARY_INTERFACE(Sub)
    BINARY_INTERFACE(Mul)
    BINARY_INTERFACE(UDiv)
    BINARY_INTERFACE(SDiv)
    BINARY_INTERFACE(URem)
    BINARY_INTERFACE(SRem)
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

    virtual ValueHandle *Select(ValueHandle *cond, ValueHandle *ifTrue, ValueHandle *ifFalse) = 0;

    virtual ValueHandle *Parameter(size_t index) = 0;
};

#endif
