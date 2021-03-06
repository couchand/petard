// types

#include "type.h"

#include <sstream>

llvm::Type *VoidTypeHandle::getLLVMType(llvm::LLVMContext &context) const
{
    return llvm::Type::getVoidTy(context);
}

std::string VoidTypeHandle::toString() const
{
    std::string name("void");
    return name;
}

llvm::Type *FunctionTypeHandle::getLLVMType(llvm::LLVMContext &context) const
{
    llvm::Type *returnType = returns->getLLVMType(context);

    std::vector<llvm::Type *> parameterTypes;
    for (unsigned i = 0, e = params.size(); i < e; i++)
    {
        parameterTypes.push_back(params[i]->getLLVMType(context));
    }

    return llvm::FunctionType::get(returnType, parameterTypes, false);
}

std::string FunctionTypeHandle::toString() const
{
    std::stringstream ss;
    ss << returns->toString() << " (";

    for (unsigned i = 0, e = params.size(); i < e; i++)
    {
        if (i != 0)
        {
            ss << ", ";
        }

        ss << params[i]->toString();
    }

    ss << ")";

    return ss.str();
}

llvm::Type *IntTypeHandle::getLLVMType(llvm::LLVMContext &context) const
{
    return llvm::Type::getIntNTy(context, numBits);
}

std::string IntTypeHandle::toString() const
{
    std::stringstream ss;
    ss << "i" << numBits;
    return ss.str();
}

llvm::Type *FloatTypeHandle::getLLVMType(llvm::LLVMContext &context) const
{
    switch (numBits)
    {
        case 16: return llvm::Type::getHalfTy(context);
        case 32: return llvm::Type::getFloatTy(context);
        case 64: return llvm::Type::getDoubleTy(context);
        default: return 0;
    }
}

std::string FloatTypeHandle::toString() const
{
    switch (numBits)
    {
        case 16: return "half";
        case 32: return "float";
        case 64: return "double";
        default: return "**ERROR: invalid float width**";
    }
}

llvm::Type *PointerTypeHandle::getLLVMType(llvm::LLVMContext &context) const
{
    return llvm::PointerType::getUnqual(pointee->getLLVMType(context));
}

std::string PointerTypeHandle::toString() const
{
    std::string pointeeStr(pointee->toString());
    return pointeeStr + "*";
}

llvm::Type *VectorTypeHandle::getLLVMType(llvm::LLVMContext &context) const
{
    return llvm::VectorType::get(element->getLLVMType(context), size);
}

std::string VectorTypeHandle::toString() const
{
    std::stringstream ss;
    ss << "<" << size << " x " << element->toString() << ">";
    return ss.str();
}

llvm::Type *ArrayTypeHandle::getLLVMType(llvm::LLVMContext &context) const
{
    return llvm::ArrayType::get(element->getLLVMType(context), size);
}

std::string ArrayTypeHandle::toString() const
{
    std::stringstream ss;
    ss << "[" << size << " x " << element->toString() << "]";
    return ss.str();
}

llvm::Type *StructTypeHandle::getLLVMType(llvm::LLVMContext &context) const
{
    std::vector<llvm::Type *> elementTypes;
    elementTypes.reserve(elements.size());

    for (unsigned i = 0, e = elements.size(); i < e; i += 1)
    {
        elementTypes.push_back(elements[i]->getLLVMType(context));
    }

    return llvm::StructType::get(context, elementTypes);
}

std::string StructTypeHandle::toString() const
{
    std::stringstream ss;
    ss << "{";

    if (elements.size())
    {
        ss << elements[0]->toString();

        for (unsigned i = 1, e = elements.size(); i < e; i += 1)
        {
            ss << ", " << elements[i]->toString();
        }
    }

    ss << "}";
    return ss.str();
}
