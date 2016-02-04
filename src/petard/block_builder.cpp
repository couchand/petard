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
    llvm::BasicBlock *child = llvm::BasicBlock::Create(context, name, parent->F);
    return new BlockBuilder(context, parent, child);
}

BlockBuilder *BlockBuilder::SplitBlock()
{
    llvm::BasicBlock *child = block->splitBasicBlock(builder.GetInsertPoint(), "merge");
    builder.SetInsertPoint(block);
    return new BlockBuilder(context, parent, child);
}

IfBuilder BlockBuilder::If(ValueHandle *condition)
{
    BlockBuilder *consequent = ChildBlock("then");
    BlockBuilder *alternate = ChildBlock("else");
    BlockBuilder *merge = SplitBlock();

    llvm::Instruction *term = block->getTerminator();
    if (term) term->eraseFromParent();

    builder.CreateCondBr(condition->getLLVMValue(), consequent->GetBlock(), alternate->GetBlock());

    consequent->InsertAfter();
    consequent->Br(merge);
    consequent->InsertBefore();

    alternate->InsertAfter();
    alternate->Br(merge);
    alternate->InsertBefore();

    builder.SetInsertPoint(merge->block);
    block = merge->block;

    return IfBuilder { consequent, alternate };
}

void BlockBuilder::Br(InstructionBuilder *dest)
{
    llvm::Instruction *term = block->getTerminator();
    if (term) term->eraseFromParent();
    builder.SetInsertPoint(block);

    llvm::BranchInst *inst = builder.CreateBr(dest->GetBlock());

    if (insertAfter) builder.SetInsertPoint(inst);
}

void BlockBuilder::Return()
{
    llvm::Instruction *term = block->getTerminator();
    if (term) term->eraseFromParent();
    builder.SetInsertPoint(block);

    llvm::ReturnInst *inst = builder.CreateRetVoid();

    if (insertAfter) builder.SetInsertPoint(inst);
}

void BlockBuilder::Return(int value)
{
    Return(MakeValue(parent->Type->returns, value));
}

void BlockBuilder::Return(ValueHandle *value)
{
    llvm::Value *returnValue = value->getLLVMValue();

    llvm::Instruction *term = block->getTerminator();
    if (term) term->eraseFromParent();
    builder.SetInsertPoint(block);

    llvm::ReturnInst *inst = builder.CreateRet(returnValue);

    if (insertAfter) builder.SetInsertPoint(inst);
}

ValueHandle *BlockBuilder::Alloca(TypeHandle *t)
{
    llvm::AllocaInst *alloca = builder.CreateAlloca(t->getLLVMType(context));
    if (insertAfter) builder.SetInsertPoint(alloca);
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

void BlockBuilder::Store(int value, ValueHandle *ptr)
{
    PointerTypeHandle *pt = static_cast<PointerTypeHandle *>(ptr->Type);
    Store(MakeValue(pt->pointee, value), ptr);
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
