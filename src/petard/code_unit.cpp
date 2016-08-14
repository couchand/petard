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

CodeUnit::CodeUnit(const char *filename) : Context(llvm::getGlobalContext())
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

    TheManager = new llvm::FunctionPassManager(TheModule);
    TheManager->add(new llvm::DataLayoutPass());
    TheManager->add(llvm::createBasicAliasAnalysisPass());
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
    output.SetUseAtomicWrites(true);

    llvm::WriteBitcodeToFile(TheModule, output);

    output.close();

    return true;
}

FunctionBuilder *CodeUnit::MakeFunction(const char *name, const FunctionTypeHandle *type)
{
    llvm::Function *f = buildFunctionHeader(name, type);

    if (!f) return 0;

    return new FunctionBuilder(name, type, Context, f);
}

FunctionValueHandle *CodeUnit::DeclareFunction(const char *name, const FunctionTypeHandle *type)
{
    llvm::Function *f = buildFunctionHeader(name, type);

    return new FunctionValueHandle(type, f);
}

ConstantValueHandle *CodeUnit::ConstantString(const std::string &value)
{
    const TypeHandle *type = new ArrayTypeHandle(value.size() + 1, new IntTypeHandle(8));

    llvm::GlobalVariable *gv = new llvm::GlobalVariable(
      *TheModule,
      type->getLLVMType(Context),
      true,                 // constant
      llvm::GlobalValue::InternalLinkage,
      llvm::ConstantDataArray::getString(Context, value)
    );

    const TypeHandle *ptrtype = new PointerTypeHandle(type);

    return new ConstantValueHandle(ptrtype, gv);
}
