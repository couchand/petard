// codeunit

#ifndef CODE_UNIT_H
#define CODE_UNIT_H

#include "type.h"
#include "value.h"
#include "function_builder.h"

#include "llvm/PassManager.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

class CodeUnit
{
    llvm::Function *buildFunctionHeader(const char *name, const FunctionTypeHandle *type);

public:
    CodeUnit(const char *filename);

    void dumpModule()
    {
        TheModule->dump();
    }

    void *JITFunction(FunctionBuilder *fn);

    bool WriteToFile(const char *filename);

    FunctionBuilder *MakeFunction(const char *name, const FunctionTypeHandle *type);
    FunctionValueHandle *DeclareFunction(const char *name, const FunctionTypeHandle *type);

    ConstantValueHandle *ConstantString(const std::string &value);

    llvm::LLVMContext &Context;
    llvm::Module *TheModule;
    llvm::FunctionPassManager *TheManager;
    llvm::ExecutionEngine *TheEngine;
};

#endif
