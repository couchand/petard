// petard, an llvm library

ffi = require('ffi');

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

    f16: petard.getFloatTy(16),
    f32: petard.getFloatTy(32),
    f64: petard.getFloatTy(64),

    arrayOf: petard.getArrayTy,
    structOf: petard.getStructTy,
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

// structured switch statement helper
var chooseimpl = function Choose(cond) {

  var that = this;

  var merge = this.splitBlock("after");

  var defaultBlock = this.createBlock("default");

  defaultBlock.insertAfter();
  defaultBlock.br(merge);
  defaultBlock.insertBefore();

  var sw = this.switch(cond, defaultBlock);

  this.useBlock(merge);

  return {
    defaultCase: defaultBlock,

    addCase: function addCase(onVal) {
      var caseBlock = that.createBlock("case" + onVal);

      caseBlock.insertAfter();
      caseBlock.br(merge);
      caseBlock.insertBefore();

      sw.addCase(onVal, caseBlock);

      return caseBlock;
    }
  };

};

attach("choose", chooseimpl);

// helper to load a jitted function

var origMakeFn = petard.CodeUnit.prototype.makeFunction;

petard.CodeUnit.prototype.makeFunction = function() {

  var fn = origMakeFn.apply(this, arguments);
  fn.codeUnit = this;
  return fn;
};

function convertToRefType(ty) {

  if (ty.isVoidType()) return 'void';
  if (ty.isIntType()) return 'int';
  if (ty.isFloatType()) return 'float';
  if (ty.isArrayType() && ty.element.isIntType() && ty.element.bitwidth == 8) return 'string';
  if (ty.isPointerType() && ty.pointee.isIntType() && ty.pointee.bitwidth == 8) return 'string';
  if (ty.isFunctionType()) {
    var ret = convertToRefType(ty.returns);
    var params = ty.parameters.map(function (p) { return convertToRefType(p); });

    if (!ret) return;
    if (!params.every(function (p) { return !!p; })) return;

    return [ret, params];
  }
}

var jitimpl = function JITFunction() {

  fnty = convertToRefType(this.type);
  if (!fnty) return;

  fn = this.codeUnit.jitFunction(this);

  return ffi.ForeignFunction(fn, fnty[0], fnty[1]);
};

attach("jitCompile", jitimpl);

// export everything
module.exports = petard;
