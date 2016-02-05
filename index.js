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

  var ifTrue = this.createBlock("then");
  ifTrue.insertAfter();
  ifTrue.br(merge);
  ifTrue.insertBefore();

  var ifFalse = this.createBlock("else");
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

// structured while statement helper
var whileimpl = function While(condFn) {

  var merge = this.splitBlock("after");
  var condCheck = this.createBlock("cond");
  var body = this.createBlock("body");
  body.insertAfter();
  body.br(condCheck);
  body.insertBefore();

  var cond = condFn(condCheck);
  condCheck.br(cond, body, merge);

  this.br(condCheck);

  this.useBlock(merge);

  return body;

};

attach("while", whileimpl);

// export everything
module.exports = petard;
