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
    virtual llvm::Type *getLLVMType(llvm::LLVMContext &context) const = 0;
    virtual std::string toString() const = 0;

    virtual bool isVoidType() const { return false; }
    virtual bool isIntType() const { return false; }
    virtual bool isFloatType() const { return false; }
    virtual bool isPointerType() const { return false; }
    virtual bool isFunctionType() const { return false; }
    virtual bool isVectorType() const { return false; }
    virtual bool isArrayType() const { return false; }
    virtual bool isStructType() const { return false; }

    virtual bool isCompatibleWith(const TypeHandle *other) const = 0;
};

class VoidTypeHandle : public TypeHandle
{
public:
    llvm::Type *getLLVMType(llvm::LLVMContext &context) const;
    std::string toString() const;

    bool isVoidType() const { return true; }

    bool isCompatibleWith(const TypeHandle *other) const { return other->isVoidType(); }
};

class FunctionTypeHandle : public TypeHandle
{
public:
    const TypeHandle *returns;
    std::vector<const TypeHandle *> params;

    FunctionTypeHandle(const TypeHandle *r, std::vector<const TypeHandle *> p)
    : returns(r), params(p) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context) const;
    std::string toString() const;

    bool isFunctionType() const { return true; }

    bool isCompatibleWith(const TypeHandle *other) const
    {
        if (!other->isFunctionType()) return false;

        const FunctionTypeHandle *otherFn = static_cast<const FunctionTypeHandle *>(other);

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
    const unsigned numBits;

    IntTypeHandle(unsigned const bitWidth)
    : numBits(bitWidth) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context) const;
    std::string toString() const;

    bool isIntType() const { return true; }

    bool isCompatibleWith(const TypeHandle *other) const
    {
        if (!other->isIntType()) return false;

        const IntTypeHandle *otherInt = static_cast<const IntTypeHandle *>(other);
        return numBits == otherInt->numBits;
    }
};

class FloatTypeHandle : public TypeHandle
{
public:
    const unsigned numBits;

    FloatTypeHandle(unsigned const bitWidth)
    : numBits(bitWidth) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context) const;
    std::string toString() const;

    bool isFloatType() const { return true; }

    bool isCompatibleWith(const TypeHandle *other) const
    {
        if (!other->isFloatType()) return false;

        const FloatTypeHandle *otherFloat = static_cast<const FloatTypeHandle *>(other);
        return numBits == otherFloat->numBits;
    }
};

class PointerTypeHandle : public TypeHandle
{
public:
    const TypeHandle *pointee;

    PointerTypeHandle(const TypeHandle *p)
    : pointee(p) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context) const;
    std::string toString() const;

    bool isPointerType() const { return true; }

    bool isCompatibleWith(const TypeHandle *other) const
    {
        if (!other->isPointerType()) return false;

        const PointerTypeHandle *otherPtr = static_cast<const PointerTypeHandle *>(other);
        return pointee->isCompatibleWith(otherPtr->pointee);
    }
};

class VectorTypeHandle : public TypeHandle
{
public:
    const unsigned size;
    const TypeHandle *element;

    VectorTypeHandle(const unsigned s, const TypeHandle *e)
    : size(s), element(e) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context) const;
    std::string toString() const;

    bool isVectorType() const { return true; }

    bool isCompatibleWith(const TypeHandle *other) const
    {
        if (!other->isVectorType()) return false;

        const VectorTypeHandle *otherVec = static_cast<const VectorTypeHandle *>(other);
        return size == otherVec->size && element->isCompatibleWith(otherVec->element);
    }
};

class ArrayTypeHandle : public TypeHandle
{
public:
    const unsigned size;
    const TypeHandle *element;

    ArrayTypeHandle(const unsigned s, const TypeHandle *e)
    : size(s), element(e) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context) const;
    std::string toString() const;

    bool isArrayType() const { return true; }

    bool isCompatibleWith(const TypeHandle *other) const
    {
        if (!other->isArrayType()) return false;

        const ArrayTypeHandle *otherArr = static_cast<const ArrayTypeHandle *>(other);
        return size == otherArr->size && element->isCompatibleWith(otherArr->element);
    }
};

class StructTypeHandle : public TypeHandle
{
public:
    std::vector<const TypeHandle *> elements;

    StructTypeHandle(std::vector<const TypeHandle *> e)
    : elements(e) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context) const;
    std::string toString() const;

    bool isStructType() const { return true; }

    bool isCompatibleWith(const TypeHandle *other) const
    {
        if (!other->isStructType()) return false;

        const StructTypeHandle *otherStruct = static_cast<const StructTypeHandle *>(other);

        if (elements.size() != otherStruct->elements.size()) return false;

        for (unsigned i = 0, e = elements.size(); i < e; i += 1)
        {
            if (!elements[i]->isCompatibleWith(otherStruct->elements[i])) return false;
        }

        return true;
    }
};

#endif
