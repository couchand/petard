// codeunit

#ifndef CODE_UNIT_H
#define CODE_UNIT_H

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
