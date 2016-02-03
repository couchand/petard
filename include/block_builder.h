// block builder

#ifndef BLOCK_BUILDER_H
#define BLOCK_BUILDER_H

#include <string>
#include <vector>

#include "llvm/IR/IRBuilder.h"

#include "types.h"
#include "value.h"

#include "builder.h"

class BlockBuilder : public InstructionBuilder
{
    llvm::IRBuilder<> builder;
    llvm::LLVMContext &context;
    FunctionBuilder *parent;
    llvm::BasicBlock *block;

    ValueHandle* callFunction(FunctionTypeHandle *fnTy, llvm::Value *fn, std::vector<ValueHandle *> args);

public:
    BlockBuilder(llvm::LLVMContext &c, FunctionBuilder *p, llvm::BasicBlock *b)
    : builder(c), context(c), parent(p), block(b)
    {
        builder.SetInsertPoint(block);
    }

    llvm::BasicBlock *GetBlock() { return block; }

    ValueHandle *MakeValue(TypeHandle *t, int i);

    BlockBuilder *ChildBlock(const char *name);

    void If(ValueHandle *condition, InstructionBuilder *consequent, InstructionBuilder *alternate);
    void Br(InstructionBuilder *dest);

    ValueHandle *LoadConstant(ValueHandle *value);

    ValueHandle *CallFunction(ValueHandle *fn, std::vector<ValueHandle *> args);
    ValueHandle *CallFunction(FunctionBuilder *fn, std::vector<ValueHandle *> args);

    void Return();
    void Return(int value);
    void Return(ValueHandle *value);

    ValueHandle *Alloca(TypeHandle *type);
    ValueHandle *Alloca(TypeHandle *type, int arraySize);
    ValueHandle *Alloca(TypeHandle *type, ValueHandle *arraySize);

    ValueHandle *Load(ValueHandle *ptr);

    void Store(int value, ValueHandle *ptr);
    void Store(ValueHandle *value, ValueHandle *ptr);

#define BINARY_HEADER(name) ValueHandle *name(ValueHandle *lhs, ValueHandle *rhs);

    BINARY_HEADER(Add)
    BINARY_HEADER(Sub)
    BINARY_HEADER(Mul)
    BINARY_HEADER(UDiv)
    BINARY_HEADER(SDiv)
    BINARY_HEADER(URem)
    BINARY_HEADER(SRem)
    BINARY_HEADER(And)
    BINARY_HEADER(Or)
    BINARY_HEADER(Xor)
    BINARY_HEADER(Shl)
    BINARY_HEADER(LShr)
    BINARY_HEADER(AShr)

    BINARY_HEADER(Equal)
    BINARY_HEADER(NotEqual)
    BINARY_HEADER(UGreaterThan)
    BINARY_HEADER(UAtLeast)
    BINARY_HEADER(ULessThan)
    BINARY_HEADER(UAtMost)
    BINARY_HEADER(SGreaterThan)
    BINARY_HEADER(SAtLeast)
    BINARY_HEADER(SLessThan)
    BINARY_HEADER(SAtMost)

    ValueHandle *Select(ValueHandle *cond, ValueHandle *ifTrue, ValueHandle *ifFalse);

    ValueHandle *Parameter(size_t index);
};

#endif
