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

ValueHandle *FunctionBuilder::Alloca(TypeHandle *t)
{
    llvm::AllocaInst *alloca = builder.CreateAlloca(t->getLLVMType(context));
    return new PlainValueHandle(new PointerTypeHandle(t), alloca);
}

ValueHandle *FunctionBuilder::Alloca(TypeHandle *t, int size)
{
    ValueHandle *s = makeValue(new IntTypeHandle(32), size);
    return Alloca(t, s);
}

ValueHandle *FunctionBuilder::Alloca(TypeHandle *t, ValueHandle *size)
{
    llvm::AllocaInst *alloca = builder.CreateAlloca(t->getLLVMType(context), size->getLLVMValue());
    return new PlainValueHandle(new PointerTypeHandle(t), alloca);
}

ValueHandle *FunctionBuilder::Load(ValueHandle *ptr)
{
    PointerTypeHandle *pt = static_cast<PointerTypeHandle *>(ptr->Type);

    llvm::LoadInst *load = builder.CreateLoad(ptr->getLLVMValue());
    return new PlainValueHandle(pt->pointee, load);
}

void FunctionBuilder::Store(int value, ValueHandle *ptr)
{
    PointerTypeHandle *pt = static_cast<PointerTypeHandle *>(ptr->Type);
    Store(makeValue(pt->pointee, value), ptr);
}

void FunctionBuilder::Store(ValueHandle *value, ValueHandle *ptr)
{
    builder.CreateStore(value->getLLVMValue(), ptr->getLLVMValue());
}

#define BINARY_BUILDER(name, factory) \
ValueHandle *FunctionBuilder::name(ValueHandle *lhs, ValueHandle *rhs) \
{ \
    TypeHandle *t = lhs->Type; /* TODO: something better */ \
    llvm::Value *val = builder.factory(lhs->getLLVMValue(), rhs->getLLVMValue()); \
    return new PlainValueHandle(t, val); \
}

BINARY_BUILDER(Add, CreateAdd)
BINARY_BUILDER(Sub, CreateSub)
BINARY_BUILDER(Mul, CreateMul)
BINARY_BUILDER(UDiv, CreateUDiv)
BINARY_BUILDER(SDiv, CreateSDiv)
BINARY_BUILDER(URem, CreateURem)
BINARY_BUILDER(SRem, CreateSRem)
BINARY_BUILDER(And, CreateAnd)
BINARY_BUILDER(Or, CreateOr)
BINARY_BUILDER(Xor, CreateXor)
BINARY_BUILDER(Shl, CreateShl)
BINARY_BUILDER(LShr, CreateLShr)
BINARY_BUILDER(AShr, CreateAShr)

#define BINARY_PREDICATE(name, intfactory) \
ValueHandle *FunctionBuilder::name(ValueHandle *lhs, ValueHandle *rhs) \
{ \
    TypeHandle *t = lhs->Type; /* TODO: unify types */ \
    llvm::Value *val; \
    if (t->isIntType()) \
    { \
        val = builder.intfactory(lhs->getLLVMValue(), rhs->getLLVMValue()); \
    } \
    else \
    { \
        return 0; \
    } \
    return new PlainValueHandle(new IntTypeHandle(1), val); \
}

BINARY_PREDICATE(Equal, CreateICmpEQ)
BINARY_PREDICATE(NotEqual, CreateICmpNE)

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

ValueHandle *FunctionBuilder::callFunction(FunctionTypeHandle *fnTy, llvm::Value *fn, std::vector<ValueHandle *> args)
{
    std::vector<llvm::Value *> argVals;
    for (unsigned i = 0, e = args.size(); i < e; i++)
    {
        argVals.push_back(args[i]->getLLVMValue());
    }

    llvm::Value *call = builder.CreateCall(fn, argVals);

    return new PlainValueHandle(fnTy->returns, call);
}

ValueHandle *FunctionBuilder::CallFunction(ValueHandle *fn, std::vector<ValueHandle *> args)
{
    FunctionTypeHandle *fnTy = static_cast<FunctionTypeHandle *>(fn->Type);
    return callFunction(fnTy, fn->getLLVMValue(), args);
}

ValueHandle *FunctionBuilder::CallFunction(FunctionBuilder *fn, std::vector<ValueHandle *> args)
{
    FunctionTypeHandle *fnTy = static_cast<FunctionTypeHandle *>(fn->Type);
    return callFunction(fnTy, fn->F, args);
}
