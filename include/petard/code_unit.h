// codeunit

#ifndef CODEGEN_H
#define CODEGEN_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/TypeBuilder.h"

#include "type.h"
#include "value.h"
#include "function_builder.h"

class CodeUnit
{
    llvm::Function *buildFunctionHeader(const char *name, FunctionTypeHandle *type);

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

    bool WriteToFile(const char *filename);

    FunctionBuilder *MakeFunction(const char *name, FunctionTypeHandle *type);
    FunctionValueHandle *DeclareFunction(const char *name, FunctionTypeHandle *type);

    ConstantValueHandle *ConstantString(const std::string &value);

    llvm::LLVMContext &Context;
    llvm::Module *TheModule;
};

#endif
