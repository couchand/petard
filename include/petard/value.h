// value holder

#ifndef VALUE_H
#define VALUE_H

#include "llvm/IR/IRBuilder.h"

#include "type.h"

class ValueHandle
{
public:
    ValueHandle(std::shared_ptr<const TypeHandle> t)
    : Type(std::move(t)) {}

    std::shared_ptr<const TypeHandle> Type;

    virtual llvm::Value *getLLVMValue() = 0;
    virtual bool isConstant() { return false; }
};

class PlainValueHandle : public ValueHandle
{
    bool isC;
public:
    PlainValueHandle(std::shared_ptr<const TypeHandle> t, llvm::Value* v, bool c = false)
    : ValueHandle(std::move(t)), isC(c), Value(v) {}

    llvm::Value *Value;

    llvm::Value *getLLVMValue();

    bool isConstant() { return isC; }
};

// TODO: rename GlobalValueHandle
class ConstantValueHandle : public ValueHandle
{
public:
    ConstantValueHandle(std::shared_ptr<const TypeHandle> t, llvm::GlobalVariable *g)
    : ValueHandle(std::move(t)), Storage(g) {}

    llvm::GlobalVariable *Storage;

    llvm::Value *getLLVMValue();

    bool isConstant() { return true; }
};

class FunctionValueHandle : public ValueHandle
{
public:
    FunctionValueHandle(std::shared_ptr<const TypeHandle> t, llvm::Function *f)
    : ValueHandle(std::move(t)), Function(f) {}

    llvm::Function *Function;

    llvm::Value *getLLVMValue();

    bool isConstant() { return true; }
};

#endif
