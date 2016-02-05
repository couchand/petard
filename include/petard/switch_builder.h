// a switch instruction with cases

#ifndef SWITCH_BUILDER_H
#define SWITCH_BUILDER_H

#include "builder.h"

class SwitchBuilder
{
    llvm::SwitchInst *inst;
    llvm::IntegerType *ty;

public:
    SwitchBuilder(llvm::SwitchInst *i) : inst(i)
    {
        ty = static_cast<llvm::IntegerType *>
        (
            inst->getCondition()->getType()
        );
    }

    void AddCase(int onVal, InstructionBuilder *dest);
};

#endif
