// value holder

#ifndef VALUE_H
#define VALUE_H

#include <string>

#include "llvm/IR/IRBuilder.h"

#include "types.h"

class ValueHandle
{
public:
    ValueHandle(TypeHandle *t)
    : Type(t) {}

    TypeHandle *Type;

    virtual llvm::Value *getLLVMValue() = 0;
};

class PlainValueHandle : public ValueHandle
{
public:
    PlainValueHandle(TypeHandle *t, llvm::Value* v)
    : ValueHandle(t), Value(v) {}

    llvm::Value *Value;

    llvm::Value *getLLVMValue();
};

class ConstantValueHandle : public ValueHandle
{
public:
    ConstantValueHandle(TypeHandle *t, llvm::GlobalVariable *g)
    : ValueHandle(t), Storage(g) {}

    llvm::GlobalVariable *Storage;

    llvm::Value *getLLVMValue();
};

class FunctionValueHandle : public ValueHandle
{
public:
    FunctionValueHandle(TypeHandle *t, llvm::Function *f)
    : ValueHandle(t), Function(f) {}

    llvm::Function *Function;

    llvm::Value *getLLVMValue();
};

#endif
