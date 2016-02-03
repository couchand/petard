// block builder

#include "block_builder.h"
#include "function_builder.h"

ValueHandle *BlockBuilder::MakeValue(TypeHandle *t, int i)
{
    llvm::Value *v = llvm::ConstantInt::get(t->getLLVMType(context), i);
    return new PlainValueHandle(t, v);
}

BlockBuilder *BlockBuilder::ChildBlock(const char *name)
{
    llvm::BasicBlock *block = llvm::BasicBlock::Create(context, name, parent->F);
    return new BlockBuilder(context, parent, block);
}

void BlockBuilder::If(ValueHandle *condition, InstructionBuilder *consequent, InstructionBuilder *alternate)
{
    BlockBuilder *merge = ChildBlock("merge");

    builder.CreateCondBr(condition->getLLVMValue(), consequent->GetBlock(), alternate->GetBlock());

    builder.SetInsertPoint(merge->block);
    block = merge->block;
}

void BlockBuilder::Br(InstructionBuilder *dest)
{
    builder.CreateBr(dest->GetBlock());
}

void BlockBuilder::Return()
{
    builder.CreateRetVoid();
}

void BlockBuilder::Return(int value)
{
    Return(MakeValue(parent->Type->returns, value));
}

void BlockBuilder::Return(ValueHandle *value)
{
    llvm::Value *returnValue = value->getLLVMValue();

    builder.CreateRet(returnValue);
}

ValueHandle *BlockBuilder::Alloca(TypeHandle *t)
{
    llvm::AllocaInst *alloca = builder.CreateAlloca(t->getLLVMType(context));
    return new PlainValueHandle(new PointerTypeHandle(t), alloca);
}

ValueHandle *BlockBuilder::Alloca(TypeHandle *t, int size)
{
    ValueHandle *s = MakeValue(new IntTypeHandle(32), size);
    return Alloca(t, s);
}

ValueHandle *BlockBuilder::Alloca(TypeHandle *t, ValueHandle *size)
{
    llvm::AllocaInst *alloca = builder.CreateAlloca(t->getLLVMType(context), size->getLLVMValue());
    return new PlainValueHandle(new PointerTypeHandle(t), alloca);
}

ValueHandle *BlockBuilder::Load(ValueHandle *ptr)
{
    PointerTypeHandle *pt = static_cast<PointerTypeHandle *>(ptr->Type);

    llvm::LoadInst *load = builder.CreateLoad(ptr->getLLVMValue());
    return new PlainValueHandle(pt->pointee, load);
}

void BlockBuilder::Store(int value, ValueHandle *ptr)
{
    PointerTypeHandle *pt = static_cast<PointerTypeHandle *>(ptr->Type);
    Store(MakeValue(pt->pointee, value), ptr);
}

void BlockBuilder::Store(ValueHandle *value, ValueHandle *ptr)
{
    builder.CreateStore(value->getLLVMValue(), ptr->getLLVMValue());
}

#define BINARY_BUILDER(name, factory) \
ValueHandle *BlockBuilder::name(ValueHandle *lhs, ValueHandle *rhs) \
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
ValueHandle *BlockBuilder::name(ValueHandle *lhs, ValueHandle *rhs) \
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
BINARY_PREDICATE(UGreaterThan, CreateICmpUGT)
BINARY_PREDICATE(UAtLeast, CreateICmpUGE)
BINARY_PREDICATE(ULessThan, CreateICmpULT)
BINARY_PREDICATE(UAtMost, CreateICmpULE)
BINARY_PREDICATE(SGreaterThan, CreateICmpSGT)
BINARY_PREDICATE(SAtLeast, CreateICmpSGE)
BINARY_PREDICATE(SLessThan, CreateICmpSLT)
BINARY_PREDICATE(SAtMost, CreateICmpSLE)

ValueHandle *BlockBuilder::Select(ValueHandle *cond, ValueHandle *ifTrue, ValueHandle *ifFalse)
{
    TypeHandle *t = ifTrue->Type; // TODO: unify types

    llvm::Value *val = builder.CreateSelect(
        cond->getLLVMValue(),
        ifTrue->getLLVMValue(),
        ifFalse->getLLVMValue()
    );

    return new PlainValueHandle(t, val);
}

ValueHandle *BlockBuilder::Parameter(size_t index)
{
    return parent->Parameter(index);
}

ValueHandle *BlockBuilder::LoadConstant(ValueHandle *value)
{
    llvm::Value *expression = builder.CreateConstGEP2_32(value->getLLVMValue(), 0, 0);

    return new PlainValueHandle(value->Type, expression);
}

ValueHandle *BlockBuilder::callFunction(FunctionTypeHandle *fnTy, llvm::Value *fn, std::vector<ValueHandle *> args)
{
    std::vector<llvm::Value *> argVals;
    for (unsigned i = 0, e = args.size(); i < e; i++)
    {
        argVals.push_back(args[i]->getLLVMValue());
    }

    llvm::Value *call = builder.CreateCall(fn, argVals);

    return new PlainValueHandle(fnTy->returns, call);
}

ValueHandle *BlockBuilder::CallFunction(ValueHandle *fn, std::vector<ValueHandle *> args)
{
    FunctionTypeHandle *fnTy = static_cast<FunctionTypeHandle *>(fn->Type);
    return callFunction(fnTy, fn->getLLVMValue(), args);
}

ValueHandle *BlockBuilder::CallFunction(FunctionBuilder *fn, std::vector<ValueHandle *> args)
{
    FunctionTypeHandle *fnTy = static_cast<FunctionTypeHandle *>(fn->Type);
    return callFunction(fnTy, fn->F, args);
}
