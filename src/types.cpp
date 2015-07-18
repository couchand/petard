// types

#include <stdio.h>

#include "types.h"

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
    std::string name(returns->toString());

    name += " (";

    for (unsigned i = 0, e = params.size(); i < e; i++)
    {
        if (i != 0)
        {
            name += ", ";
        }

        name += params[i]->toString();
    }

    name += ")";

    return name;
}

llvm::Type *IntTypeHandle::getLLVMType(llvm::LLVMContext &context)
{
    return llvm::Type::getIntNTy(context, numBits);
}

std::string IntTypeHandle::toString()
{
    // The number 9 is based on the assumption that the current (2015)
    // value for MAX_INT_BITS won't be increased any time soon.  It
    // is 8388607, which is seven digits, plus 'i' for eight, plus the
    // null terminator. TODO: not this
    char raw[9];
    sprintf(raw, "i%i", numBits);
    std::string name(raw);
    return name;
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
    char sizeBuf[10];
    sprintf(sizeBuf, "%i", size);
    std::string count(sizeBuf);
    std::string elTy(element->toString());

    std::string arrayStr("[");
    arrayStr += count;
    arrayStr += " x ";
    arrayStr += elTy;
    arrayStr += "]";

    return arrayStr;
}
