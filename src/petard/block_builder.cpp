// block builder

#include "block_builder.h"
#include "function_builder.h"
#include "switch_builder.h"

#include "llvm_utils.h"

std::shared_ptr<ValueHandle> BlockBuilder::MakeValue(std::shared_ptr<const TypeHandle> t, double i)
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

    return std::make_shared<PlainValueHandle>(std::move(t), v, true);
}

std::shared_ptr<ValueHandle> BlockBuilder::MakeUndefined(std::shared_ptr<const TypeHandle> t)
{
    llvm::Value *v = llvm::UndefValue::get(t->getLLVMType(context));
    if (!v) return 0;

    return std::make_shared<PlainValueHandle>(std::move(t), v, true);
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

void BlockBuilder::CondBr(std::shared_ptr<ValueHandle> condition, InstructionBuilder *ifTrue, InstructionBuilder *ifFalse)
{
    RemoveTerminator();

    llvm::BranchInst *inst = builder.CreateCondBr(condition->getLLVMValue(), ifTrue->GetBlock(), ifFalse->GetBlock());

    if (insertAfter) builder.SetInsertPoint(inst);
}

SwitchBuilder *BlockBuilder::Switch(std::shared_ptr<ValueHandle> condition, InstructionBuilder *defaultDest)
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

void BlockBuilder::Return(std::shared_ptr<ValueHandle> value)
{
    llvm::Value *returnValue = value->getLLVMValue();

    RemoveTerminator();

    llvm::ReturnInst *inst = builder.CreateRet(returnValue);

    if (insertAfter) builder.SetInsertPoint(inst);
}

std::shared_ptr<ValueHandle> BlockBuilder::Alloca(std::shared_ptr<const TypeHandle> t)
{
    llvm::AllocaInst *alloca = builder.CreateAlloca(t->getLLVMType(context));
    if (insertAfter) builder.SetInsertPoint(alloca);
    return std::make_shared<PlainValueHandle>(std::make_shared<PointerTypeHandle>(std::move(t)), alloca);
}

std::shared_ptr<ValueHandle> BlockBuilder::Alloca(std::shared_ptr<const TypeHandle> t, std::shared_ptr<ValueHandle> size)
{
    llvm::AllocaInst *alloca = builder.CreateAlloca(t->getLLVMType(context), size->getLLVMValue());
    if (insertAfter) builder.SetInsertPoint(alloca);
    return std::make_shared<PlainValueHandle>(std::make_shared<PointerTypeHandle>(std::move(t)), alloca);
}

std::shared_ptr<ValueHandle> BlockBuilder::Load(std::shared_ptr<ValueHandle> ptr)
{
    const PointerTypeHandle *pt = static_cast<const PointerTypeHandle *>(ptr->Type.get());

    llvm::LoadInst *load = builder.CreateLoad(ptr->getLLVMValue());
    if (insertAfter) builder.SetInsertPoint(load);
    return std::make_shared<PlainValueHandle>(pt->pointee, load);
}

void BlockBuilder::Store(std::shared_ptr<ValueHandle> value, std::shared_ptr<ValueHandle> ptr)
{
    llvm::StoreInst *store = builder.CreateStore(value->getLLVMValue(), ptr->getLLVMValue());
    if (insertAfter) builder.SetInsertPoint(store);
}

#define BINARY_BUILDER(name, factory) \
std::shared_ptr<ValueHandle> BlockBuilder::name(std::shared_ptr<ValueHandle> lhs, std::shared_ptr<ValueHandle> rhs) \
{ \
    std::shared_ptr<const TypeHandle> t = lhs->Type; /* TODO: something better */ \
    llvm::Value *val = builder.factory(lhs->getLLVMValue(), rhs->getLLVMValue()); \
    return std::make_shared<PlainValueHandle>(std::move(t), val); \
}

#define BINARY_BUILDER2(name, intfactory, floatfactory) \
std::shared_ptr<ValueHandle> BlockBuilder::name(std::shared_ptr<ValueHandle> lhs, std::shared_ptr<ValueHandle> rhs) \
{ \
    std::shared_ptr<const TypeHandle> t = lhs->Type; \
    const TypeHandle *rt = rhs->Type.get(); \
    if (!t->isCompatibleWith(rt)) \
    { \
        return 0; \
    } \
    const TypeHandle *et = t.get(); \
    if (et->isVectorType()) \
    { \
        const VectorTypeHandle *vt = static_cast<const VectorTypeHandle *>(et); \
        et = vt->element.get(); \
    } \
    llvm::Value *val; \
    if (et->isIntType()) \
    { \
        val = builder.intfactory(lhs->getLLVMValue(), rhs->getLLVMValue()); \
    } \
    else if (et->isFloatType()) \
    { \
        val = builder.floatfactory(lhs->getLLVMValue(), rhs->getLLVMValue()); \
    } \
    else \
    { \
        return 0; \
    } \
    return std::make_shared<PlainValueHandle>(std::move(t), val); \
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
std::shared_ptr<ValueHandle> BlockBuilder::name(std::shared_ptr<ValueHandle> lhs, std::shared_ptr<ValueHandle> rhs) \
{ \
    llvm::Value *val = builder.factory(lhs->getLLVMValue(), rhs->getLLVMValue()); \
    return std::make_shared<PlainValueHandle>(std::make_shared<IntTypeHandle>(1), val); \
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

#define CAST_BUILDER(name, factory) std::shared_ptr<ValueHandle> BlockBuilder::name(std::shared_ptr<ValueHandle> value, std::shared_ptr<const TypeHandle> type) \
{ \
    llvm::Value *val = builder.factory(value->getLLVMValue(), type->getLLVMType(context)); \
    return std::make_shared<PlainValueHandle>(std::move(type), val); \
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
CAST_BUILDER(PtrToInt, CreatePtrToInt)
CAST_BUILDER(IntToPtr, CreateIntToPtr)
CAST_BUILDER(Bitcast, CreateBitCast)

std::shared_ptr<ValueHandle> BlockBuilder::Select(std::shared_ptr<ValueHandle> cond, std::shared_ptr<ValueHandle> ifTrue, std::shared_ptr<ValueHandle> ifFalse)
{
    std::shared_ptr<const TypeHandle> t = ifTrue->Type; // TODO: unify types

    llvm::Value *val = builder.CreateSelect(
        cond->getLLVMValue(),
        ifTrue->getLLVMValue(),
        ifFalse->getLLVMValue()
    );

    return std::make_shared<PlainValueHandle>(std::move(t), val);
}

std::shared_ptr<ValueHandle> BlockBuilder::Parameter(size_t index)
{
    return parent->Parameter(index);
}

std::shared_ptr<ValueHandle> BlockBuilder::LoadConstant(std::shared_ptr<ValueHandle> value)
{
    if (!value->Type->isPointerType())
    {
        return 0;
    }

    const PointerTypeHandle *ptrty = static_cast<const PointerTypeHandle *>(value->Type.get());

    std::shared_ptr<const IntTypeHandle> thirtyTwo = std::make_shared<IntTypeHandle>(32);
    std::vector<std::shared_ptr<ValueHandle>> idxs;
    idxs.push_back(MakeValue(thirtyTwo, 0));
    std::shared_ptr<const TypeHandle> elty = getElementType(ptrty->pointee.get(), idxs);

    llvm::Value *expression = builder.CreateConstGEP2_32(value->getLLVMValue(), 0, 0);

    return std::make_shared<PlainValueHandle>(std::make_shared<PointerTypeHandle>(elty), expression);
}

std::vector<std::shared_ptr<ValueHandle>> getRest(std::vector<std::shared_ptr<ValueHandle>> vec)
{
    std::vector<std::shared_ptr<ValueHandle>> rest;
    rest.reserve(vec.size() - 1);

    for (unsigned i = 1, e = vec.size(); i < e; i += 1)
    {
        rest.push_back(std::move(vec[i]));
    }

    return rest;
}

std::shared_ptr<ValueHandle> BlockBuilder::GetElementPointer(std::shared_ptr<ValueHandle> ptr, std::vector<std::shared_ptr<ValueHandle>> idxs)
{
    unsigned count = idxs.size();

    const PointerTypeHandle *ptrTy = 0;

    if (ptr->Type->isVectorType())
    {
        const VectorTypeHandle *vecTy = static_cast<const VectorTypeHandle *>(ptr->Type.get());

        if (vecTy->element->isPointerType())
        {
            ptrTy = static_cast<const PointerTypeHandle *>(vecTy->element.get());
        }
    }
    else if (ptr->Type->isPointerType())
    {
        ptrTy = static_cast<const PointerTypeHandle *>(ptr->Type.get());
    }

    if (!ptrTy)
    {
        return 0;
    }

    std::shared_ptr<const TypeHandle> elty = count <= 1 ? ptrTy->pointee : getElementType(ptrTy->pointee.get(), getRest(idxs));

    if (!elty)
    {
        return 0;
    }
    std::shared_ptr<const TypeHandle> newty = std::make_shared<PointerTypeHandle>(std::move(elty));

    if (ptr->Type->isVectorType())
    {
        const VectorTypeHandle *vecTy = static_cast<const VectorTypeHandle *>(ptr->Type.get());
        newty = std::make_shared<VectorTypeHandle>(vecTy->size, std::move(newty));
    }

    std::vector<llvm::Value *> idxlist;
    idxlist.reserve(count);

    for (std::shared_ptr<ValueHandle> idx : idxs)
    {
        llvm::Value *idxValue = idx->getLLVMValue();

        if (idx->Type->isVectorType())
        {
            const VectorTypeHandle *vt = static_cast<const VectorTypeHandle *>(idx->Type.get());
            if (!newty->isVectorType())
            {
                newty = std::make_shared<VectorTypeHandle>(vt->size, std::move(newty));
            }
            else
            {
                const VectorTypeHandle *nt = static_cast<const VectorTypeHandle *>(newty.get());
                if (vt->size != nt->size)
                {
                    return 0;
                }
            }
        }

        idxlist.push_back(idxValue);
    }

    llvm::Value *ptrVal = ptr->getLLVMValue();

    llvm::Value *elPtr = builder.CreateGEP(ptrVal, idxlist);

    return std::make_shared<PlainValueHandle>(std::move(newty), elPtr);
}

std::shared_ptr<const TypeHandle> BlockBuilder::getElementType(const TypeHandle *ty, std::vector<std::shared_ptr<ValueHandle>> idxs)
{
    if (idxs.size() == 0)
    {
        return 0; /* TODO: something better */
    }

    const bool recurse = idxs.size() > 1;

    std::shared_ptr<ValueHandle> first = idxs[0];
    std::vector<std::shared_ptr<ValueHandle>> rest = getRest(idxs);

    if (ty->isPointerType())
    {
        const PointerTypeHandle *ptrTy = static_cast<const PointerTypeHandle *>(ty);
        return recurse ? getElementType(ptrTy->pointee.get(), rest) : ptrTy->pointee;
    }

    if (ty->isVectorType())
    {
        const VectorTypeHandle *vecTy = static_cast<const VectorTypeHandle *>(ty);
        return recurse ? getElementType(vecTy->element.get(), rest) : vecTy->element;
    }

    if (ty->isArrayType())
    {
        const ArrayTypeHandle *arrTy = static_cast<const ArrayTypeHandle *>(ty);
        return recurse ? getElementType(arrTy->element.get(), rest) : arrTy->element;
    }

    if (ty->isStructType())
    {
        if (!first->isConstant()) return 0;

        const StructTypeHandle *strucTy = static_cast<const StructTypeHandle *>(ty);
        llvm::ConstantInt *idx = static_cast<llvm::ConstantInt *>(first->getLLVMValue());

        // TODO: check that value is in range
        unsigned idxVal = (unsigned)idx->getLimitedValue();
        return recurse ? getElementType(strucTy->elements[idxVal].get(), rest) : strucTy -> elements[idxVal];
    }

    // error, we can't dive any further in
    return 0;
}

std::shared_ptr<ValueHandle> BlockBuilder::ExtractElement(std::shared_ptr<ValueHandle> vec, std::shared_ptr<ValueHandle> idx)
{
    if (!idx->Type->isIntType() || !vec->Type->isVectorType())
    {
        return 0;
    }

    const VectorTypeHandle *vecTy = static_cast<const VectorTypeHandle *>(vec->Type.get());

    llvm::Value *res = builder.CreateExtractElement(vec->getLLVMValue(), idx->getLLVMValue());
    if (!res) return 0;

    return std::make_shared<PlainValueHandle>(vecTy->element, res);
}

std::shared_ptr<ValueHandle> BlockBuilder::InsertElement(std::shared_ptr<ValueHandle> vec, std::shared_ptr<ValueHandle> val, std::shared_ptr<ValueHandle> idx)
{
    if (!idx->Type->isIntType() || !vec->Type->isVectorType())
    {
        return 0;
    }

    const VectorTypeHandle *vecTy = static_cast<const VectorTypeHandle *>(vec->Type.get());

    if (!val->Type->isCompatibleWith(vecTy->element.get()))
    {
        return 0;
    }

    llvm::Value *res = builder.CreateInsertElement(vec->getLLVMValue(), val->getLLVMValue(), idx->getLLVMValue());
    if (!res) return 0;

    return std::make_shared<PlainValueHandle>(vec->Type, res);
}

std::shared_ptr<ValueHandle> BlockBuilder::callFunction(const FunctionTypeHandle *fnTy, llvm::Value *fn, std::vector<std::shared_ptr<ValueHandle>> args)
{
    std::vector<llvm::Value *> argVals;
    for (unsigned i = 0, e = args.size(); i < e; i++)
    {
        argVals.push_back(args[i]->getLLVMValue());
    }

    llvm::Value *call = builder.CreateCall(fn, argVals);

    return std::make_shared<PlainValueHandle>(fnTy->returns, call);
}

std::shared_ptr<ValueHandle> BlockBuilder::CallFunction(std::shared_ptr<ValueHandle> fn, std::vector<std::shared_ptr<ValueHandle>> args)
{
    const FunctionTypeHandle *fnTy = static_cast<const FunctionTypeHandle *>(fn->Type.get());
    return callFunction(fnTy, fn->getLLVMValue(), args);
}

std::shared_ptr<ValueHandle> BlockBuilder::CallFunction(FunctionBuilder *fn, std::vector<std::shared_ptr<ValueHandle>> args)
{
    const FunctionTypeHandle *fnTy = static_cast<const FunctionTypeHandle *>(fn->Type.get());
    return callFunction(fnTy, fn->F, args);
}
