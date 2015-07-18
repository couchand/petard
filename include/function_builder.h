// function builder

#ifndef FUNCTION_BUILDER_H
#define FUNCTION_BUILDER_H

#include <string>

#include "llvm/IR/IRBuilder.h"

class FunctionBuilder
{
public:
    FunctionBuilder(const char *name, llvm::Function *f)
    : Name(name), F(f) {}

    std::string Name;
    llvm::Function *F;
};

#endif
