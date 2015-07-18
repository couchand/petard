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

std::string VoidTypeHandle::toString(int nestLevel)
{
    return toString();
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
    return toString(0);
}

std::string FunctionTypeHandle::toString(int nestLevel)
{
    std::string name;

    for (unsigned i = 0, e = params.size(); i < e; i++)
    {
        if (i != 0)
        {
            name += " ";
        }

        name += params[i]->toString(nestLevel + 1);
    }

    if (params.size() > 0)
    {
        name += " ";
    }

    name += "->";

    std::string ret = returns->toString(nestLevel + 1);

    if (ret != "void")
    {
        name += " " + ret;
    }

    if (nestLevel > 0)
    {
        return "(" + name + ")";
    }

    return name;
}

llvm::Type *IntTypeHandle::getLLVMType(llvm::LLVMContext &context)
{
    return llvm::Type::getIntNTy(context, numBits);
}

std::string IntTypeHandle::toString()
{
    // The number 11 is based on the assumption that the current (2015)
    // value for MAX_INT_BITS won't be increased any time soon.  It
    // is 8388607, which is seven digits, plus 'int' for ten, plus the
    // null terminator. TODO: not this
    char raw[11];
    sprintf(raw, "int%i", numBits);
    std::string name(raw);
    return name;
}

std::string IntTypeHandle::toString(int nestLevel)
{
    return toString();
}
