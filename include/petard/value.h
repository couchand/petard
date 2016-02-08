// value holder

#ifndef VALUE_H
#define VALUE_H

#include "llvm/IR/IRBuilder.h"

#include "type.h"

class ValueHandle
{
public:
    ValueHandle(TypeHandle *t)
    : Type(t) {}

    TypeHandle *Type;

    virtual llvm::Value *getLLVMValue() = 0;
    virtual bool isConstant() { return false; }
};

class PlainValueHandle : public ValueHandle
{
    bool isC;
public:
    PlainValueHandle(TypeHandle *t, llvm::Value* v, bool c = false)
    : ValueHandle(t), isC(c), Value(v) {}

    llvm::Value *Value;

    llvm::Value *getLLVMValue();

    bool isConstant() { return isC; }
};

// TODO: rename GlobalValueHandle
class ConstantValueHandle : public ValueHandle
{
public:
    ConstantValueHandle(TypeHandle *t, llvm::GlobalVariable *g)
    : ValueHandle(t), Storage(g) {}

    llvm::GlobalVariable *Storage;

    llvm::Value *getLLVMValue();

    bool isConstant() { return true; }
};

class FunctionValueHandle : public ValueHandle
{
public:
    FunctionValueHandle(TypeHandle *t, llvm::Function *f)
    : ValueHandle(t), Function(f) {}

    llvm::Function *Function;

    llvm::Value *getLLVMValue();

    bool isConstant() { return true; }
};

#endif
