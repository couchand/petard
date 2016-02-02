module.exports = codegen = require('bindings')('codegen');

codegen.type = {

    void: codegen.getVoidTy(),

    i1: codegen.getIntTy(1),
    i8: codegen.getIntTy(8),
    i16: codegen.getIntTy(16),
    i32: codegen.getIntTy(32),
    i64: codegen.getIntTy(64),

    pointerTo: codegen.getPointerTy

};
