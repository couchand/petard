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

    llvm::BasicBlock *GetBlock() { return builder->GetBlock(); }
    void InsertAfter() { builder->InsertAfter(); }
    void InsertBefore() { builder->InsertBefore(); }

    ValueHandle *MakeValue(TypeHandle *t, int i);

    BlockBuilder *ChildBlock(const char *name);

    IfBuilder If(ValueHandle *condition);
    void Br(InstructionBuilder *dest);
    void CondBr(ValueHandle *condition, InstructionBuilder *ifTrue, InstructionBuilder *ifFalse);

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
