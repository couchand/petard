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

    virtual bool isIntType() { return false; }
    virtual bool isPointerType() { return false; }
    virtual bool isFunctionType() { return false; }
    virtual bool isArrayType() { return false; }
};

class VoidTypeHandle : public TypeHandle
{
public:
    llvm::Type *getLLVMType(llvm::LLVMContext &context);
    std::string toString();
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

    bool isFunctionType() { return true; }
};

class IntTypeHandle : public TypeHandle
{
public:
    unsigned const numBits;

    IntTypeHandle(unsigned bitWidth)
    : numBits(bitWidth) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context);
    std::string toString();

    bool isIntType() { return true; }
};

class PointerTypeHandle : public TypeHandle
{
public:
    TypeHandle *pointee;

    PointerTypeHandle(TypeHandle *p)
    : pointee(p) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context);
    std::string toString();

    bool isPointerType() { return true; }
};

class ArrayTypeHandle : public TypeHandle
{
public:
    unsigned size;
    TypeHandle *element;

    ArrayTypeHandle(unsigned s, TypeHandle *e)
    : size(s), element(e) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context);
    std::string toString();

    bool isArrayType() { return true; }
};

#endif
