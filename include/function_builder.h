// function builder

#ifndef FUNCTION_BUILDER_H
#define FUNCTION_BUILDER_H

#include <string>

#include "llvm/IR/IRBuilder.h"

#include "types.h"

class FunctionBuilder
{
public:
    FunctionBuilder(const char *name, TypeHandle *t, llvm::Function *f)
    : Name(name), Type(t), F(f) {}

    std::string Name;
    TypeHandle *Type;
    llvm::Function *F;
};

#endif
