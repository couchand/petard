// block builder

#ifndef BLOCK_BUILDER_H
#define BLOCK_BUILDER_H

#include <vector>

#include "type.h"
#include "value.h"
#include "builder.h"
#include "switch_builder.h"

class BlockBuilder : public InstructionBuilder
{
    llvm::IRBuilder<> builder;
    llvm::LLVMContext &context;
    FunctionBuilder *parent;
    llvm::BasicBlock *block;
    bool insertAfter = false;

    ValueHandle* callFunction(const FunctionTypeHandle *fnTy, llvm::Value *fn, std::vector<ValueHandle *> args);

public:
    BlockBuilder(llvm::LLVMContext &c, FunctionBuilder *p, llvm::BasicBlock *b)
    : builder(c), context(c), parent(p), block(b)
    {
        builder.SetInsertPoint(block);
    }

    FunctionBuilder *GetParent() { return parent; }
    llvm::BasicBlock *GetBlock() { return block; }

    void InsertAfter() { insertAfter = true; }
    void InsertBefore() { insertAfter = false; }

    ValueHandle *MakeValue(const TypeHandle *t, double i);
    ValueHandle *MakeUndefined(const TypeHandle *t);

    BlockBuilder *ChildBlock(const char *name);
    BlockBuilder *SplitBlock(const char *name);
    void UseBlock(InstructionBuilder *replacement);
    void RemoveTerminator();

    void Br(InstructionBuilder *dest);
    void CondBr(ValueHandle *condition, InstructionBuilder *ifTrue, InstructionBuilder *ifFalse);
    SwitchBuilder *Switch(ValueHandle *condition, InstructionBuilder *defaultDest);

    ValueHandle *LoadConstant(ValueHandle *value);
    ValueHandle *GetElementPointer(ValueHandle *ptr, std::vector<ValueHandle *> idxs);
    const TypeHandle *getElementType(const TypeHandle *ty, std::vector<ValueHandle *> idxs);
    ValueHandle *ExtractElement(ValueHandle *vec, ValueHandle *idx);
    ValueHandle *InsertElement(ValueHandle *vec, ValueHandle *val, ValueHandle *idx);

    ValueHandle *CallFunction(ValueHandle *fn, std::vector<ValueHandle *> args);
    ValueHandle *CallFunction(FunctionBuilder *fn, std::vector<ValueHandle *> args);

    void Return();
    void Return(ValueHandle *value);

    ValueHandle *Alloca(const TypeHandle *type);
    ValueHandle *Alloca(const TypeHandle *type, ValueHandle *arraySize);

    ValueHandle *Load(ValueHandle *ptr);

    void Store(ValueHandle *value, ValueHandle *ptr);

#define BINARY_HEADER(name) ValueHandle *name(ValueHandle *lhs, ValueHandle *rhs);

    BINARY_HEADER(Add)
    BINARY_HEADER(Sub)
    BINARY_HEADER(Mul)
    BINARY_HEADER(UDiv)
    BINARY_HEADER(SDiv)
    BINARY_HEADER(FDiv)
    BINARY_HEADER(URem)
    BINARY_HEADER(SRem)
    BINARY_HEADER(FRem)
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

    BINARY_HEADER(FOEqual)
    BINARY_HEADER(FONotEqual)
    BINARY_HEADER(FOGreaterThan)
    BINARY_HEADER(FOAtLeast)
    BINARY_HEADER(FOLessThan)
    BINARY_HEADER(FOAtMost)
    BINARY_HEADER(FUEqual)
    BINARY_HEADER(FUNotEqual)
    BINARY_HEADER(FUGreaterThan)
    BINARY_HEADER(FUAtLeast)
    BINARY_HEADER(FULessThan)
    BINARY_HEADER(FUAtMost)

#define CAST_HEADER(name) virtual ValueHandle *name(ValueHandle *value, const TypeHandle *type);

    CAST_HEADER(Trunc)
    CAST_HEADER(ZExt)
    CAST_HEADER(SExt)
    CAST_HEADER(FPToUI)
    CAST_HEADER(FPToSI)
    CAST_HEADER(UIToFP)
    CAST_HEADER(SIToFP)
    CAST_HEADER(FPTrunc)
    CAST_HEADER(FPExt)
    CAST_HEADER(PtrToInt)
    CAST_HEADER(IntToPtr)
    CAST_HEADER(Bitcast)

    ValueHandle *Select(ValueHandle *cond, ValueHandle *ifTrue, ValueHandle *ifFalse);

    ValueHandle *Parameter(size_t index);
};

#endif
