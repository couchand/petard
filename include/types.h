// llvm types

#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/TypeBuilder.h"

class TypeHandle
{
public:
    virtual llvm::Type *getLLVMType(llvm::LLVMContext &context) = 0;
    virtual std::string toString() = 0;
    virtual std::string toString(int nestLevel) = 0;
};

class VoidTypeHandle : public TypeHandle
{
public:
    llvm::Type *getLLVMType(llvm::LLVMContext &context);
    std::string toString();
    std::string toString(int nestLevel);
};

class FunctionTypeHandle : public TypeHandle
{
public:
    TypeHandle *returns;
    std::vector<TypeHandle *> params;

    FunctionTypeHandle(TypeHandle *r, std::vector<TypeHandle *> p)
    : returns(r), params(p) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context);
    std::string toString();
    std::string toString(int nestLevel);
};

class IntTypeHandle : public TypeHandle
{
public:
    unsigned const numBits;

    IntTypeHandle(unsigned bitWidth)
    : numBits(bitWidth) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context);
    std::string toString();
    std::string toString(int nestLevel);
};

#endif
