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
};

class ConstantValueHandle : public ValueHandle
{
public:
    ConstantValueHandle(TypeHandle *t, llvm::GlobalVariable *g)
    : ValueHandle(t), Storage(g) {}

    llvm::GlobalVariable *Storage;
};

#endif
