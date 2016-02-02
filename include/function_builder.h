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

public:
    FunctionBuilder(const char *name, FunctionTypeHandle *t, llvm::LLVMContext &c, llvm::Function *f)
    : builder(c), context(c), Name(name), Type(t), F(f)
    {
        llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", F);
        builder.SetInsertPoint(entry);
    }

    std::string Name;
    FunctionTypeHandle *Type;
    llvm::Function *F;

    ValueHandle *LoadConstant(ValueHandle *value);

    ValueHandle *CallFunction(ValueHandle *fn, std::vector<ValueHandle *> args);

    void Return();
    void Return(int value);
};

#endif
