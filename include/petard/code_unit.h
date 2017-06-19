// codeunit

#ifndef CODE_UNIT_H
#define CODE_UNIT_H

#include <memory>

#include "type.h"
#include "value.h"
#include "function_builder.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Transforms/Scalar/GVN.h"

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

    FunctionBuilder *MakeFunction(const char *name, std::shared_ptr<const FunctionTypeHandle> type);
    std::shared_ptr<FunctionValueHandle> DeclareFunction(const char *name, std::shared_ptr<const FunctionTypeHandle> type);

    std::shared_ptr<ConstantValueHandle> ConstantString(const std::string &value);

    llvm::LLVMContext Context;
    llvm::Module *TheModule;
    llvm::legacy::FunctionPassManager *TheManager;
    llvm::ExecutionEngine *TheEngine;
};

#endif
