// switch implementation

#include "switch_builder.h"

void SwitchBuilder::AddCase(int onVal, InstructionBuilder *dest)
{
    llvm::ConstantInt *onVal2 = llvm::ConstantInt::get(ty, onVal);
    inst->addCase(onVal2, dest->GetBlock());
}
