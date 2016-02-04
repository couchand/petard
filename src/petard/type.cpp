// types

#include <sstream>

#include "type.h"

llvm::Type *VoidTypeHandle::getLLVMType(llvm::LLVMContext &context)
{
    return llvm::Type::getVoidTy(context);
}

std::string VoidTypeHandle::toString()
{
    std::string name("void");
    return name;
}

llvm::Type *FunctionTypeHandle::getLLVMType(llvm::LLVMContext &context)
{
    llvm::Type *returnType = returns->getLLVMType(context);

    std::vector<llvm::Type *> parameterTypes;
    for (unsigned i = 0, e = params.size(); i < e; i++)
    {
        parameterTypes.push_back(params[i]->getLLVMType(context));
    }

    return llvm::FunctionType::get(returnType, parameterTypes, false);
}

std::string FunctionTypeHandle::toString()
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

llvm::Type *IntTypeHandle::getLLVMType(llvm::LLVMContext &context)
{
    return llvm::Type::getIntNTy(context, numBits);
}

std::string IntTypeHandle::toString()
{
    std::stringstream ss;
    ss << "i" << numBits;
    return ss.str();
}

llvm::Type *PointerTypeHandle::getLLVMType(llvm::LLVMContext &context)
{
    return llvm::PointerType::getUnqual(pointee->getLLVMType(context));
}

std::string PointerTypeHandle::toString()
{
    std::string pointeeStr(pointee->toString());
    return pointeeStr + "*";
}

llvm::Type *ArrayTypeHandle::getLLVMType(llvm::LLVMContext &context)
{
    return llvm::ArrayType::get(element->getLLVMType(context), size);
}

std::string ArrayTypeHandle::toString()
{
    std::stringstream ss;
    ss << "[" << size << " x " << element->toString() << "]";
    return ss.str();
}
