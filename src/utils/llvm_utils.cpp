// some llvm utilities

#include "llvm_utils.h"

void pruneOrphanedSuccessors(llvm::BasicBlock *block, bool alreadyPruning)
{
    llvm::TerminatorInst *term = block->getTerminator();
    if (!term)
    {
        if (!alreadyPruning) return; // no orphans to prune

        // the end of a chain being pruned
        block->eraseFromParent();
        return;
    }

    for (unsigned i = 0, e = term->getNumSuccessors(); i < e; i += 1)
    {
        llvm::BasicBlock *successor = term->getSuccessor(i);

        llvm::BasicBlock *onlyPredecessor = successor->getSinglePredecessor();

        if (!onlyPredecessor) return; // not an orphan

        // if we got here, it's an orphan

        // remove terminator to recursively clean up
        llvm_utils::RemoveTerminator(successor);

        // now remove the block
        successor->eraseFromParent();
    }
}

namespace llvm_utils
{
    void RemoveTerminator(llvm::BasicBlock *block)
    {
        pruneOrphanedSuccessors(block, false);

        llvm::Instruction *term = block->getTerminator();
        if (term) term->eraseFromParent();
    }
}
