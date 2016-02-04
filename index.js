// petard, an llvm library

// load the native module
var petard = require('bindings')('petard');

// basic type shortcuts
petard.type = {

    void: petard.getVoidTy(),

    i1: petard.getIntTy(1),
    i8: petard.getIntTy(8),
    i16: petard.getIntTy(16),
    i32: petard.getIntTy(32),
    i64: petard.getIntTy(64),

    pointerTo: petard.getPointerTy

};

// some built-in macros to help

// attach a function to the builder prototypes
function attach(name, action) {
    petard.Builder.prototype[name] = action;
    petard.FunctionBuilder.prototype[name] = action;
}

// structured if statement helper
var ifimpl = function If(cond) {

  var merge = this.splitBlock("merge");

  ifTrue = this.createBlock("then");
  ifTrue.insertAfter();
  ifTrue.br(merge);
  ifTrue.insertBefore();

  ifFalse = this.createBlock("else");
  ifFalse.insertAfter();
  ifFalse.br(merge);
  ifFalse.insertBefore();

  this.br(cond, ifTrue, ifFalse);

  this.useBlock(merge);

  return {
    "then": ifTrue,
    "else": ifFalse
  };

};

attach("if", ifimpl);

// export everything
module.exports = petard;
