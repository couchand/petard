// function builder

#include "function_builder.h"

void FunctionBuilder::Return()
{
    builder.CreateRetVoid();
}

void FunctionBuilder::Return(int value)
{
    llvm::Value *returnValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), value);

    builder.CreateRet(returnValue);
}

ValueHandle *FunctionBuilder::LoadConstant(ValueHandle *value)
{
    llvm::Value *expression = builder.CreateConstGEP2_32(value->getLLVMValue(), 0, 0);

    return new PlainValueHandle(value->Type, expression);
}

ValueHandle *FunctionBuilder::CallFunction(ValueHandle *fn, std::vector<ValueHandle *> args)
{
    llvm::Value *callee = fn->getLLVMValue();

    std::vector<llvm::Value *> argVals;
    for (unsigned i = 0, e = args.size(); i < e; i++)
    {
        argVals.push_back(args[i]->getLLVMValue());
    }

    llvm::Value *call = builder.CreateCall(callee, argVals);

    FunctionTypeHandle *fnTy = static_cast<FunctionTypeHandle *>(fn->Type);
    return new PlainValueHandle(fnTy->returns, call);
}
