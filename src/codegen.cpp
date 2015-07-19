// codegen

#include "codegen.h"

llvm::Function *CodeUnit::buildFunctionHeader(const char *name, TypeHandle *type)
{
    std::string myName(name);

    llvm::GlobalValue::LinkageTypes linkage = llvm::Function::ExternalLinkage;
    llvm::FunctionType *ft = (llvm::FunctionType *)type->getLLVMType(Context);
    llvm::Function *f = llvm::Function::Create(ft, linkage, myName, TheModule);

    if (f->getName() != myName)
    {
        f->eraseFromParent();
        return 0;  // TODO not this
    }

    return f;
}

FunctionBuilder *CodeUnit::MakeFunction(const char *name, TypeHandle *type)
{
    llvm::Function *f = buildFunctionHeader(name, type);

    return new FunctionBuilder(name, type, Context, f);
}

FunctionValueHandle *CodeUnit::DeclareFunction(const char *name, TypeHandle *type)
{
    llvm::Function *f = buildFunctionHeader(name, type);

    return new FunctionValueHandle(type, f);
}

ConstantValueHandle *CodeUnit::ConstantString(const std::string &value)
{
    TypeHandle *type = new ArrayTypeHandle(value.size() + 1, new IntTypeHandle(8));

    llvm::GlobalVariable *gv = new llvm::GlobalVariable(
      *TheModule,
      type->getLLVMType(Context),
      true,                 // constant
      llvm::GlobalValue::InternalLinkage,
      llvm::ConstantDataArray::getString(Context, value)
    );

    return new ConstantValueHandle(type, gv);
}
