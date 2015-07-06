// codegen

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/TypeBuilder.h"

class TypeHandle
{
public:
    virtual llvm::Type *getLLVMType(llvm::LLVMContext &context) = 0;
    virtual std::string toString() = 0;
    virtual std::string toString(int nestLevel) = 0;
};

class FunctionBuilder
{
public:
    FunctionBuilder(const char *name, llvm::Function *f)
    : Name(name), F(f) {}

    std::string Name;
    llvm::Function *F;
};

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

class VoidTypeHandle : public TypeHandle
{
public:
    llvm::Type *getLLVMType(llvm::LLVMContext &context);
    std::string toString();
    std::string toString(int nestLevel);
};

class FunctionTypeHandle : public TypeHandle
{
public:
    TypeHandle *returns;
    std::vector<TypeHandle *> params;

    FunctionTypeHandle(TypeHandle *r, std::vector<TypeHandle *> p)
    : returns(r), params(p) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context);
    std::string toString();
    std::string toString(int nestLevel);
};
