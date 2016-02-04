// value holder

#include "value.h"

llvm::Value *ConstantValueHandle::getLLVMValue()
{
    return Storage;
}

llvm::Value *FunctionValueHandle::getLLVMValue()
{
    return Function;
}

llvm::Value *PlainValueHandle::getLLVMValue()
{
    return Value;
}
