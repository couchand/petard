// some llvm utilities

#ifndef LLVM_UTILS_H
#define LLVM_UTILS_H

#include "llvm/IR/IRBuilder.h"

namespace llvm_utils
{
    void RemoveTerminator(llvm::BasicBlock *block, bool prune = true);
}

#endif
