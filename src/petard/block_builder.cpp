// block builder

 #include <iostream>

#include "block_builder.h"
#include "function_builder.h"
#include "switch_builder.h"

#include "llvm_utils.h"

ValueHandle *BlockBuilder::MakeValue(TypeHandle *t, double i)
{
    llvm::Value *v;

    if (t->isIntType())
    {
        v = llvm::ConstantInt::get(t->getLLVMType(context), i);
    }
    else if (t->isFloatType())
    {
        v = llvm::ConstantFP::get(t->getLLVMType(context), i);
    }
    else
    {
        return 0;
    }

    return new PlainValueHandle(t, v);
}

BlockBuilder *BlockBuilder::ChildBlock(const char *name)
{
    llvm::BasicBlock *child = llvm::BasicBlock::Create(context, name, parent->F);
    return new BlockBuilder(context, parent, child);
}

BlockBuilder *BlockBuilder::SplitBlock(const char *name)
{
    llvm::BasicBlock *child = block->splitBasicBlock(builder.GetInsertPoint(), name);
    llvm_utils::RemoveTerminator(block, false); // don't prune away new child
    builder.SetInsertPoint(block);
    return new BlockBuilder(context, parent, child);
}

void BlockBuilder::UseBlock(InstructionBuilder *replacement)
{
    block = replacement->GetBlock();
    builder.SetInsertPoint(block);
}

void BlockBuilder::RemoveTerminator()
{
    llvm_utils::RemoveTerminator(block);
    builder.SetInsertPoint(block);
}

void BlockBuilder::Br(InstructionBuilder *dest)
{
    RemoveTerminator();

    llvm::BranchInst *inst = builder.CreateBr(dest->GetBlock());

    if (insertAfter) builder.SetInsertPoint(inst);
}

void BlockBuilder::CondBr(ValueHandle *condition, InstructionBuilder *ifTrue, InstructionBuilder *ifFalse)
{
    RemoveTerminator();

    llvm::BranchInst *inst = builder.CreateCondBr(condition->getLLVMValue(), ifTrue->GetBlock(), ifFalse->GetBlock());

    if (insertAfter) builder.SetInsertPoint(inst);
}

SwitchBuilder *BlockBuilder::Switch(ValueHandle *condition, InstructionBuilder *defaultDest)
{
    RemoveTerminator();

    llvm::SwitchInst *inst = builder.CreateSwitch(condition->getLLVMValue(), defaultDest->GetBlock());

    if (insertAfter) builder.SetInsertPoint(inst);

    return new SwitchBuilder(inst);
}

void BlockBuilder::Return()
{
    RemoveTerminator();

    llvm::ReturnInst *inst = builder.CreateRetVoid();

    if (insertAfter) builder.SetInsertPoint(inst);
}

void BlockBuilder::Return(ValueHandle *value)
{
    llvm::Value *returnValue = value->getLLVMValue();

    RemoveTerminator();

    llvm::ReturnInst *inst = builder.CreateRet(returnValue);

    if (insertAfter) builder.SetInsertPoint(inst);
}

ValueHandle *BlockBuilder::Alloca(TypeHandle *t)
{
    llvm::AllocaInst *alloca = builder.CreateAlloca(t->getLLVMType(context));
    if (insertAfter) builder.SetInsertPoint(alloca);
    return new PlainValueHandle(new PointerTypeHandle(t), alloca);
}

ValueHandle *BlockBuilder::Alloca(TypeHandle *t, ValueHandle *size)
{
    llvm::AllocaInst *alloca = builder.CreateAlloca(t->getLLVMType(context), size->getLLVMValue());
    if (insertAfter) builder.SetInsertPoint(alloca);
    return new PlainValueHandle(new PointerTypeHandle(t), alloca);
}

ValueHandle *BlockBuilder::Load(ValueHandle *ptr)
{
    PointerTypeHandle *pt = static_cast<PointerTypeHandle *>(ptr->Type);

    llvm::LoadInst *load = builder.CreateLoad(ptr->getLLVMValue());
    if (insertAfter) builder.SetInsertPoint(load);
    return new PlainValueHandle(pt->pointee, load);
}

void BlockBuilder::Store(ValueHandle *value, ValueHandle *ptr)
{
    llvm::StoreInst *store = builder.CreateStore(value->getLLVMValue(), ptr->getLLVMValue());
    if (insertAfter) builder.SetInsertPoint(store);
}

#define BINARY_BUILDER(name, factory) \
ValueHandle *BlockBuilder::name(ValueHandle *lhs, ValueHandle *rhs) \
{ \
    TypeHandle *t = lhs->Type; /* TODO: something better */ \
    llvm::Value *val = builder.factory(lhs->getLLVMValue(), rhs->getLLVMValue()); \
    return new PlainValueHandle(t, val); \
}

#define BINARY_BUILDER2(name, intfactory, floatfactory) \
ValueHandle *BlockBuilder::name(ValueHandle *lhs, ValueHandle *rhs) \
{ \
    TypeHandle *t = lhs->Type; /* TODO: something better */ \
    llvm::Value *val; \
    if (t->isIntType()) \
    { \
        val = builder.intfactory(lhs->getLLVMValue(), rhs->getLLVMValue()); \
    } \
    else if (t->isFloatType()) \
    { \
        val = builder.floatfactory(lhs->getLLVMValue(), rhs->getLLVMValue()); \
    } \
    else \
    { \
        return 0; \
    } \
    return new PlainValueHandle(t, val); \
}

BINARY_BUILDER2(Add, CreateAdd, CreateFAdd)
BINARY_BUILDER2(Sub, CreateSub, CreateFSub)
BINARY_BUILDER2(Mul, CreateMul, CreateFMul)
BINARY_BUILDER(UDiv, CreateUDiv)
BINARY_BUILDER(SDiv, CreateSDiv)
BINARY_BUILDER(FDiv, CreateFDiv)
BINARY_BUILDER(URem, CreateURem)
BINARY_BUILDER(SRem, CreateSRem)
BINARY_BUILDER(FRem, CreateFRem)
BINARY_BUILDER(And, CreateAnd)
BINARY_BUILDER(Or, CreateOr)
BINARY_BUILDER(Xor, CreateXor)
BINARY_BUILDER(Shl, CreateShl)
BINARY_BUILDER(LShr, CreateLShr)
BINARY_BUILDER(AShr, CreateAShr)

#define BINARY_PREDICATE(name, factory) \
ValueHandle *BlockBuilder::name(ValueHandle *lhs, ValueHandle *rhs) \
{ \
    llvm::Value *val = builder.factory(lhs->getLLVMValue(), rhs->getLLVMValue()); \
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

BINARY_PREDICATE(FOEqual, CreateFCmpOEQ)
BINARY_PREDICATE(FONotEqual, CreateFCmpONE)
BINARY_PREDICATE(FOGreaterThan, CreateFCmpOGT)
BINARY_PREDICATE(FOAtLeast, CreateFCmpOGE)
BINARY_PREDICATE(FOLessThan, CreateFCmpOLT)
BINARY_PREDICATE(FOAtMost, CreateFCmpOLE)
BINARY_PREDICATE(FUEqual, CreateFCmpUEQ)
BINARY_PREDICATE(FUNotEqual, CreateFCmpUNE)
BINARY_PREDICATE(FUGreaterThan, CreateFCmpUGT)
BINARY_PREDICATE(FUAtLeast, CreateFCmpUGE)
BINARY_PREDICATE(FULessThan, CreateFCmpULT)
BINARY_PREDICATE(FUAtMost, CreateFCmpULE)

#define CAST_BUILDER(name, factory) ValueHandle *BlockBuilder::name(ValueHandle *value, TypeHandle *type) \
{ \
    llvm::Value *val = builder.factory(value->getLLVMValue(), type->getLLVMType(context)); \
    return new PlainValueHandle(type, val); \
}

CAST_BUILDER(Trunc, CreateTrunc)
CAST_BUILDER(ZExt, CreateZExt)
CAST_BUILDER(SExt, CreateSExt)
CAST_BUILDER(FPToUI, CreateFPToUI)
CAST_BUILDER(FPToSI, CreateFPToSI)
CAST_BUILDER(UIToFP, CreateUIToFP)
CAST_BUILDER(SIToFP, CreateSIToFP)
CAST_BUILDER(FPTrunc, CreateFPTrunc)
CAST_BUILDER(FPExt, CreateFPExt)

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

ValueHandle *BlockBuilder::GetElementPointer(ValueHandle *ptr, std::vector<ValueHandle *> idxs)
{
    unsigned count = idxs.size();

    if (!ptr->Type->isPointerType()) return 0;

    PointerTypeHandle *ptrTy = static_cast<PointerTypeHandle *>(ptr->Type);
    TypeHandle *elty = getElementType(ptrTy->pointee, count - 1);

    if (!elty) return 0;
    TypeHandle *newty = new PointerTypeHandle(elty);

    std::vector<llvm::Value *> idxlist;
    idxlist.reserve(count);

    for (ValueHandle *idx : idxs)
    {
        llvm::Value *idxValue = idx->getLLVMValue();
        idxlist.push_back(idxValue);
    }

    llvm::Value *ptrVal = ptr->getLLVMValue();

    llvm::Value *elPtr = builder.CreateGEP(ptrVal, idxlist);

    return new PlainValueHandle(newty, elPtr);
}

TypeHandle *BlockBuilder::getElementType(TypeHandle *ty, unsigned idxCount)
{
    if (idxCount == 0)
    {
        return ty;
    }

    if (ty->isPointerType())
    {
        PointerTypeHandle *ptrTy = static_cast<PointerTypeHandle *>(ty);
        return getElementType(ptrTy->pointee, idxCount - 1);
    }

    if (ty->isArrayType())
    {
        ArrayTypeHandle *arrTy = static_cast<ArrayTypeHandle *>(ty);
        return getElementType(arrTy->element, idxCount - 1);
    }

    // error, we can't dive any further in
    return 0;
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
