// llvm types

#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/TypeBuilder.h"

class TypeHandle
{
public:
    virtual llvm::Type *getLLVMType(llvm::LLVMContext &context) = 0;
    virtual std::string toString() = 0;

    virtual bool isVoidType() { return false; }
    virtual bool isIntType() { return false; }
    virtual bool isFloatType() { return false; }
    virtual bool isPointerType() { return false; }
    virtual bool isFunctionType() { return false; }
    virtual bool isArrayType() { return false; }
    virtual bool isStructType() { return false; }

    virtual bool isCompatibleWith(TypeHandle *other) = 0;
};

class VoidTypeHandle : public TypeHandle
{
public:
    llvm::Type *getLLVMType(llvm::LLVMContext &context);
    std::string toString();

    bool isVoidType() { return true; }

    bool isCompatibleWith(TypeHandle *other) { return other->isVoidType(); }
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

    bool isCompatibleWith(TypeHandle *other)
    {
        if (!other->isFunctionType()) return false;

        FunctionTypeHandle *otherFn = static_cast<FunctionTypeHandle *>(other);

        if (!returns->isCompatibleWith(otherFn->returns)) return false;

        if (params.size() != otherFn->params.size()) return false;

        for (unsigned i = 0, e = params.size(); i < e; i += 1)
        {
            if (!params[i]->isCompatibleWith(otherFn->params[i])) return false;
        }

        return true;
    }
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

    bool isCompatibleWith(TypeHandle *other)
    {
        if (!other->isIntType()) return false;

        IntTypeHandle *otherInt = static_cast<IntTypeHandle *>(other);
        return numBits == otherInt->numBits;
    }
};

class FloatTypeHandle : public TypeHandle
{
public:
    unsigned const numBits;

    FloatTypeHandle(unsigned bitWidth)
    : numBits(bitWidth) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context);
    std::string toString();

    bool isFloatType() { return true; }

    bool isCompatibleWith(TypeHandle *other)
    {
        if (!other->isFloatType()) return false;

        FloatTypeHandle *otherFloat = static_cast<FloatTypeHandle *>(other);
        return numBits == otherFloat->numBits;
    }
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

    bool isCompatibleWith(TypeHandle *other)
    {
        if (!other->isPointerType()) return false;

        PointerTypeHandle *otherPtr = static_cast<PointerTypeHandle *>(other);
        return pointee->isCompatibleWith(otherPtr->pointee);
    }
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

    bool isCompatibleWith(TypeHandle *other)
    {
        if (!other->isArrayType()) return false;

        ArrayTypeHandle *otherArr = static_cast<ArrayTypeHandle *>(other);
        return size == otherArr->size && element->isCompatibleWith(otherArr->element);
    }
};

class StructTypeHandle : public TypeHandle
{
public:
    std::vector<TypeHandle *> elements;

    StructTypeHandle(std::vector<TypeHandle *> e)
    : elements(e) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context);
    std::string toString();

    bool isStructType() { return true; }

    bool isCompatibleWith(TypeHandle *other)
    {
        if (!other->isStructType()) return false;

        StructTypeHandle *otherStruct = static_cast<StructTypeHandle *>(other);

        if (elements.size() != otherStruct->elements.size()) return false;

        for (unsigned i = 0, e = elements.size(); i < e; i += 1)
        {
            if (!elements[i]->isCompatibleWith(otherStruct->elements[i])) return false;
        }

        return true;
    }
};

#endif
