// function builder

#include "function_builder.h"

ValueHandle *FunctionBuilder::makeValue(TypeHandle *t, int i)
{
    llvm::Value *v = llvm::ConstantInt::get(t->getLLVMType(context), i);
    return new PlainValueHandle(t, v);
}

void FunctionBuilder::Return()
{
    builder.CreateRetVoid();
}

void FunctionBuilder::Return(int value)
{
    Return(makeValue(Type->returns, value));
}

void FunctionBuilder::Return(ValueHandle *value)
{
    llvm::Value *returnValue = value->getLLVMValue();

    builder.CreateRet(returnValue);
}

ValueHandle *FunctionBuilder::Parameter(size_t index)
{
    if (index >= parameters.size())
    {
        return 0;
    }
    TypeHandle *t = Type->params[index];
    return new PlainValueHandle(t, parameters[index]);
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
