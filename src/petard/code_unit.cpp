// codeunit

#include <string>
#include <iostream>

#include "code_unit.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Bitcode/BitstreamWriter.h"
#include "llvm/Bitcode/ReaderWriter.h"

#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"

#include "llvm/Support/TargetSelect.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Transforms/Scalar.h"

CodeUnit::CodeUnit(const char *filename)
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    std::unique_ptr<llvm::Module> owner = llvm::make_unique<llvm::Module>(filename, Context);
    TheModule = owner.get();

    std::string err;
    TheEngine = llvm::EngineBuilder(std::move(owner))
        .setErrorStr(&err)
        .setMCJITMemoryManager(llvm::make_unique<llvm::SectionMemoryManager>())
        .create();

    if (!TheEngine)
    {
        std::cerr << "ERROR: could not create ExecutionEngine: " << err << std::endl;
        // TODO: not this
        exit(1);
    }

    TheModule->setDataLayout(TheEngine->getDataLayout());

    TheManager = new llvm::legacy::FunctionPassManager(TheModule);
    TheManager->add(llvm::createBasicAAWrapperPass());
    TheManager->add(llvm::createInstructionCombiningPass());
    TheManager->add(llvm::createReassociatePass());
    TheManager->add(llvm::createGVNPass());
    TheManager->add(llvm::createCFGSimplificationPass());
    TheManager->doInitialization();
}

void *CodeUnit::JITFunction(FunctionBuilder *fn)
{
    TheEngine->finalizeObject();
    return TheEngine->getPointerToFunction(fn->F);
}

llvm::Function *CodeUnit::buildFunctionHeader(const char *name, const FunctionTypeHandle *type)
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

    llvm::WriteBitcodeToFile(TheModule, output);

    output.close();

    return true;
}

FunctionBuilder *CodeUnit::MakeFunction(const char *name, std::shared_ptr<const FunctionTypeHandle> type)
{
    llvm::Function *f = buildFunctionHeader(name, type.get());

    if (!f) return 0;

    return new FunctionBuilder(name, std::move(type), Context, f);
}

std::shared_ptr<FunctionValueHandle> CodeUnit::DeclareFunction(const char *name, std::shared_ptr<const FunctionTypeHandle> type)
{
    llvm::Function *f = buildFunctionHeader(name, type.get());

    return std::make_shared<FunctionValueHandle>(std::move(type), f);
}

std::shared_ptr<ConstantValueHandle> CodeUnit::ConstantString(const std::string &value)
{
    std::shared_ptr<const TypeHandle> type = std::make_shared<ArrayTypeHandle>(
      value.size() + 1,
      std::move(std::make_shared<IntTypeHandle>(8))
    );

    llvm::GlobalVariable *gv = new llvm::GlobalVariable(
      *TheModule,
      type->getLLVMType(Context),
      true,                 // constant
      llvm::GlobalValue::InternalLinkage,
      llvm::ConstantDataArray::getString(Context, value)
    );

    std::shared_ptr<const TypeHandle> ptrtype = std::make_shared<PointerTypeHandle>(type);

    return std::make_shared<ConstantValueHandle>(std::move(ptrtype), gv);
}
