// llvm types

#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <memory>

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
    std::shared_ptr<const TypeHandle> returns;
    std::vector<std::shared_ptr<const TypeHandle>> params;

    FunctionTypeHandle(std::shared_ptr<const TypeHandle> r, std::vector<std::shared_ptr<const TypeHandle>> p)
    : returns(std::move(r)), params(std::move(p)) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context) const;
    std::string toString() const;

    bool isFunctionType() const { return true; }

    bool isCompatibleWith(const TypeHandle *other) const
    {
        if (!other->isFunctionType()) return false;

        const FunctionTypeHandle *otherFn = static_cast<const FunctionTypeHandle *>(other);

        if (!returns->isCompatibleWith(otherFn->returns.get())) return false;

        if (params.size() != otherFn->params.size()) return false;

        for (unsigned i = 0, e = params.size(); i < e; i += 1)
        {
            if (!params[i]->isCompatibleWith(otherFn->params[i].get())) return false;
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
    std::shared_ptr<const TypeHandle> pointee;

    PointerTypeHandle(std::shared_ptr<const TypeHandle> p)
    : pointee(std::move(p)) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context) const;
    std::string toString() const;

    bool isPointerType() const { return true; }

    bool isCompatibleWith(const TypeHandle *other) const
    {
        if (!other->isPointerType()) return false;

        const PointerTypeHandle *otherPtr = static_cast<const PointerTypeHandle *>(other);
        return pointee->isCompatibleWith(otherPtr->pointee.get());
    }
};

class VectorTypeHandle : public TypeHandle
{
public:
    const unsigned size;
    std::shared_ptr<const TypeHandle> element;

    VectorTypeHandle(const unsigned s, std::shared_ptr<const TypeHandle> e)
    : size(s), element(std::move(e)) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context) const;
    std::string toString() const;

    bool isVectorType() const { return true; }

    bool isCompatibleWith(const TypeHandle *other) const
    {
        if (!other->isVectorType()) return false;

        const VectorTypeHandle *otherVec = static_cast<const VectorTypeHandle *>(other);
        return size == otherVec->size && element->isCompatibleWith(otherVec->element.get());
    }
};

class ArrayTypeHandle : public TypeHandle
{
public:
    const unsigned size;
    std::shared_ptr<const TypeHandle> element;

    ArrayTypeHandle(const unsigned s, std::shared_ptr<const TypeHandle> e)
    : size(s), element(std::move(e)) {}

    llvm::Type *getLLVMType(llvm::LLVMContext &context) const;
    std::string toString() const;

    bool isArrayType() const { return true; }

    bool isCompatibleWith(const TypeHandle *other) const
    {
        if (!other->isArrayType()) return false;

        const ArrayTypeHandle *otherArr = static_cast<const ArrayTypeHandle *>(other);
        return size == otherArr->size && element->isCompatibleWith(otherArr->element.get());
    }
};

class StructTypeHandle : public TypeHandle
{
public:
    std::vector<std::shared_ptr<const TypeHandle>> elements;

    StructTypeHandle(std::vector<std::shared_ptr<const TypeHandle>> e)
    : elements(std::move(e)) {}

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
            if (!elements[i]->isCompatibleWith(otherStruct->elements[i].get())) return false;
        }

        return true;
    }
};

#endif
