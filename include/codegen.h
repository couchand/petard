// codegen

#ifndef CODEGEN_H
#define CODEGEN_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/TypeBuilder.h"

#include "types.h"
#include "value.h"
#include "function_builder.h"

class CodeUnit
{
    llvm::Function *buildFunctionHeader(const char *name, TypeHandle *type);

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
    FunctionValueHandle *DeclareFunction(const char *name, TypeHandle *type);

    ConstantValueHandle *ConstantString(const std::string &value);

    llvm::LLVMContext &Context;
    llvm::Module *TheModule;
};

#endif
