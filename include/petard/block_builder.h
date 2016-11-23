// block builder

#ifndef BLOCK_BUILDER_H
#define BLOCK_BUILDER_H

#include <vector>
#include <memory>

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

    std::shared_ptr<ValueHandle> callFunction(const FunctionTypeHandle *fnTy, llvm::Value *fn, std::vector<std::shared_ptr<ValueHandle>> args);

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

    std::shared_ptr<ValueHandle> MakeValue(std::shared_ptr<const TypeHandle> t, double i);
    std::shared_ptr<ValueHandle> MakeUndefined(std::shared_ptr<const TypeHandle> t);

    BlockBuilder *ChildBlock(const char *name);
    BlockBuilder *SplitBlock(const char *name);
    void UseBlock(InstructionBuilder *replacement);
    void RemoveTerminator();

    void Br(InstructionBuilder *dest);
    void CondBr(std::shared_ptr<ValueHandle> condition, InstructionBuilder *ifTrue, InstructionBuilder *ifFalse);
    SwitchBuilder *Switch(std::shared_ptr<ValueHandle> condition, InstructionBuilder *defaultDest);

    std::shared_ptr<ValueHandle> LoadConstant(std::shared_ptr<ValueHandle> value);
    std::shared_ptr<ValueHandle> GetElementPointer(std::shared_ptr<ValueHandle> ptr, std::vector<std::shared_ptr<ValueHandle>> idxs);
    std::shared_ptr<const TypeHandle> getElementType(const TypeHandle *ty, std::vector<std::shared_ptr<ValueHandle>> idxs);
    std::shared_ptr<ValueHandle> ExtractElement(std::shared_ptr<ValueHandle> vec, std::shared_ptr<ValueHandle> idx);
    std::shared_ptr<ValueHandle> InsertElement(std::shared_ptr<ValueHandle> vec, std::shared_ptr<ValueHandle> val, std::shared_ptr<ValueHandle> idx);

    std::shared_ptr<ValueHandle> CallFunction(std::shared_ptr<ValueHandle> fn, std::vector<std::shared_ptr<ValueHandle>> args);
    std::shared_ptr<ValueHandle> CallFunction(FunctionBuilder *fn, std::vector<std::shared_ptr<ValueHandle>> args);

    void Return();
    void Return(std::shared_ptr<ValueHandle> value);

    std::shared_ptr<ValueHandle> Alloca(std::shared_ptr<const TypeHandle> type);
    std::shared_ptr<ValueHandle> Alloca(std::shared_ptr<const TypeHandle> type, std::shared_ptr<ValueHandle> arraySize);

    std::shared_ptr<ValueHandle> Load(std::shared_ptr<ValueHandle> ptr);

    void Store(std::shared_ptr<ValueHandle> value, std::shared_ptr<ValueHandle> ptr);

#define BINARY_HEADER(name) std::shared_ptr<ValueHandle> name(std::shared_ptr<ValueHandle> lhs, std::shared_ptr<ValueHandle> rhs);

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

#define CAST_HEADER(name) virtual std::shared_ptr<ValueHandle> name(std::shared_ptr<ValueHandle> value, std::shared_ptr<const TypeHandle> type);

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

    std::shared_ptr<ValueHandle> Select(std::shared_ptr<ValueHandle> cond, std::shared_ptr<ValueHandle> ifTrue, std::shared_ptr<ValueHandle> ifFalse);

    std::shared_ptr<ValueHandle> Parameter(size_t index);
};

#endif
