// codegen

#include <iostream>

#include "codegen.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Bitcode/BitstreamWriter.h"
#include "llvm/Bitcode/ReaderWriter.h"

llvm::Function *CodeUnit::buildFunctionHeader(const char *name, FunctionTypeHandle *type)
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

bool CodeUnit::WriteToFile(const char *name)
{
    std::error_code ec;
    llvm::raw_fd_ostream output(name, ec, (llvm::sys::fs::OpenFlags)1); // don't overwrite files
    if (ec.message() != "" && ec.message() != "Success") // brain dead
    {
        // TODO: not this
        std::cout << ec.value() << ": " << ec.message() << " [" << ec.category().name() << "]" << std::endl;
        return false;
    }
    output.SetUseAtomicWrites(true);

    llvm::WriteBitcodeToFile(TheModule, output);

    output.close();

    return true;
}

FunctionBuilder *CodeUnit::MakeFunction(const char *name, FunctionTypeHandle *type)
{
    llvm::Function *f = buildFunctionHeader(name, type);

    if (!f) return 0;

    return new FunctionBuilder(name, type, Context, f);
}

FunctionValueHandle *CodeUnit::DeclareFunction(const char *name, FunctionTypeHandle *type)
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
