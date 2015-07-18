// codegen

#include "codegen.h"

FunctionBuilder *CodeUnit::MakeFunction(const char *name, TypeHandle *type)
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

    return new FunctionBuilder(name, type, f);
}

ConstantValueHandle *CodeUnit::ConstantString(const std::string &value)
{
    llvm::ArrayType *ty = llvm::ArrayType::get(llvm::Type::getInt8Ty(Context), value.size()+1);
    llvm::GlobalVariable *gv = new llvm::GlobalVariable(
      *TheModule, ty, true, llvm::GlobalValue::InternalLinkage,
      llvm::ConstantDataArray::getString(Context, value), "str"
    );

    TypeHandle *type = new PointerTypeHandle(new IntTypeHandle(8));

    return new ConstantValueHandle(type, gv);
}
