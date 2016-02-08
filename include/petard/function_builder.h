// function builder

#ifndef FUNCTION_BUILDER_H
#define FUNCTION_BUILDER_H

#include <string>
#include <vector>

#include "llvm/IR/Module.h"

#include "type.h"
#include "value.h"
#include "builder.h"
#include "block_builder.h"
#include "switch_builder.h"

class CodeUnit;

class FunctionBuilder : public InstructionBuilder
{
    llvm::LLVMContext &context;
    BlockBuilder *builder;

    std::vector<llvm::Value *> parameters;

public:
    FunctionBuilder(const char *name, FunctionTypeHandle *t, llvm::LLVMContext &c, llvm::Function *f)
    : context(c), Name(name), Type(t), F(f)
    {
        llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", F);
        builder = new BlockBuilder(context, this, entry);

        parameters.reserve(F->arg_size());

        for (llvm::Function::arg_iterator AI = F->arg_begin()
            ,AE = F->arg_end()
            ;AI != AE
            ;++AI)
        {
            parameters.push_back(AI);
        }
    }

    std::string Name;
    FunctionTypeHandle *Type;
    llvm::Function *F;

    FunctionBuilder *GetParent() { return this; }
    llvm::BasicBlock *GetBlock() { return builder->GetBlock(); }
    void InsertAfter() { builder->InsertAfter(); }
    void InsertBefore() { builder->InsertBefore(); }

    ValueHandle *MakeValue(TypeHandle *t, double i);

    BlockBuilder *ChildBlock(const char *name);
    BlockBuilder *SplitBlock(const char *name);
    void UseBlock(InstructionBuilder *replacement);
    void RemoveTerminator();

    void Br(InstructionBuilder *dest);
    void CondBr(ValueHandle *condition, InstructionBuilder *ifTrue, InstructionBuilder *ifFalse);
    SwitchBuilder *Switch(ValueHandle *condition, InstructionBuilder *defaultDest);

    ValueHandle *LoadConstant(ValueHandle *value);
    ValueHandle *GetElementPointer(ValueHandle *ptr, std::vector<ValueHandle *> idxs);

    ValueHandle *CallFunction(ValueHandle *fn, std::vector<ValueHandle *> args);
    ValueHandle *CallFunction(FunctionBuilder *fn, std::vector<ValueHandle *> args);

    void Return();
    void Return(ValueHandle *value);

    ValueHandle *Alloca(TypeHandle *type);
    ValueHandle *Alloca(TypeHandle *type, ValueHandle *arraySize);

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

#define CAST_HEADER(name) virtual ValueHandle *name(ValueHandle *value, TypeHandle *type);

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
