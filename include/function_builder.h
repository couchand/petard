// function builder

#ifndef FUNCTION_BUILDER_H
#define FUNCTION_BUILDER_H

#include <string>
#include <vector>

#include "llvm/IR/IRBuilder.h"

#include "types.h"
#include "value.h"

class FunctionBuilder
{
    llvm::IRBuilder<> builder;
    llvm::LLVMContext &context;

    std::vector<llvm::Value *> parameters;

    ValueHandle* callFunction(FunctionTypeHandle *fnTy, llvm::Value *fn, std::vector<ValueHandle *> args);

public:
    FunctionBuilder(const char *name, FunctionTypeHandle *t, llvm::LLVMContext &c, llvm::Function *f)
    : builder(c), context(c), Name(name), Type(t), F(f)
    {
        llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", F);
        builder.SetInsertPoint(entry);

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

    ValueHandle *makeValue(TypeHandle *t, int i);

    ValueHandle *LoadConstant(ValueHandle *value);

    ValueHandle *CallFunction(ValueHandle *fn, std::vector<ValueHandle *> args);
    ValueHandle *CallFunction(FunctionBuilder *fn, std::vector<ValueHandle *> args);

    void Return();
    void Return(int value);
    void Return(ValueHandle *value);

    ValueHandle *Alloca(TypeHandle *type);
    ValueHandle *Alloca(TypeHandle *type, int arraySize);
    ValueHandle *Alloca(TypeHandle *type, ValueHandle* arraySize);

    ValueHandle *Parameter(size_t index);
};

#endif
