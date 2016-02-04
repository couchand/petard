// some llvm utilities

#include "llvm_utils.h"

namespace llvm_utils
{
    void RemoveTerminator(llvm::BasicBlock *block)
    {
        llvm::Instruction *term = block->getTerminator();
        if (term) term->eraseFromParent();
    }
}
