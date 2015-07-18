// codegen

#ifndef CODEGEN_H
#define CODEGEN_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/TypeBuilder.h"

#include "types.h"
#include "function_builder.h"

class CodeUnit
{
public:
    CodeUnit(const char *filename)
    : Context(llvm::getGlobalContext())
    {
        TheModule = new llvm::Module(filename, Context);
    }

    void dumpModule()
    {
        TheModule->dump();
    }

    FunctionBuilder *MakeFunction(const char *name, TypeHandle *type);

    llvm::LLVMContext &Context;
    llvm::Module *TheModule;
};

#endif
