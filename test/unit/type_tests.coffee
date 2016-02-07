# test the types

llvm = require '../../'

{should} = require '../helper'

describe 'getters', ->
  describe 'getVoidTy', ->
    it 'returns a type', ->
      me = llvm.getVoidTy()
      me.constructor.name.should.equal 'Type'

  describe 'getFunctionTy', ->
    it 'returns a type', ->
      me = llvm.getFunctionTy()
      me.constructor.name.should.equal 'Type'

  describe 'getIntTy', ->
    it 'returns a type', ->
      me = llvm.getIntTy 32
      me.constructor.name.should.equal 'Type'

    it 'expects a bit width', ->
      (-> llvm.getIntTy()).should.throw /bit width/i

    it 'expects a valid bit width', ->
      (-> llvm.getIntTy 'foobar').should.throw /bit width/i

  describe 'getFloatTy', ->
    it 'returns a type', ->
      me = llvm.getFloatTy 32
      me.constructor.name.should.equal 'Type'

    it 'expects a bit width', ->
      (-> llvm.getFloatTy()).should.throw /bit width/i

    it 'expects a valid bit width', ->
      (-> llvm.getFloatTy 33).should.throw /bit width/i

  describe 'getPointerTy', ->
    it 'returns a type', ->
      me = llvm.getPointerTy llvm.getIntTy 32
      me.constructor.name.should.equal 'Type'

    it 'expects a pointee type', ->
      (-> llvm.getPointerTy()).should.throw /pointee/i

  describe 'getArrayTy', ->
    it 'returns a type', ->
      me = llvm.getArrayTy 3, llvm.getIntTy 1
      me.constructor.name.should.equal 'Type'

    it 'expects an element count', ->
      (-> llvm.getArrayTy()).should.throw /size/i

    it 'expects an element type', ->
      (-> llvm.getArrayTy 3).should.throw /element/i

  describe 'getStructTy', ->
    it 'returns a type', ->
      i32 = llvm.getIntTy 32
      me = llvm.getStructTy [i32, i32, i32]
      me.constructor.name.should.equal 'Type'

    it 'expects an array of element types', ->
      (-> llvm.getStructTy()).should.throw /element/i
      (-> llvm.getStructTy 42).should.throw /element/i
      (-> llvm.getStructTy llvm.getIntTy 32).should.throw /element/i
      (-> llvm.getStructTy [42]).should.throw /element/i

describe 'VoidType', ->
  describe 'toString', ->
    it 'returns void', ->
      me = llvm.getVoidTy()
      me.toString().should.equal 'void'

  describe 'isCompatibleWith', ->
    it 'returns true for void type', ->
      me = llvm.getVoidTy()
      other = llvm.getVoidTy()
      me.isCompatibleWith(other).should.be.true

    it 'returns false for non-void types', ->
      me = llvm.getVoidTy()

      intTy = llvm.getIntTy 32
      me.isCompatibleWith(intTy).should.be.false

      floatTy = llvm.getFloatTy 32
      me.isCompatibleWith(floatTy).should.be.false

      ptrTy = llvm.getPointerTy llvm.getIntTy 32
      me.isCompatibleWith(ptrTy).should.be.false

      arrTy = llvm.getArrayTy 3, llvm.getIntTy 32
      me.isCompatibleWith(arrTy).should.be.false

      structTy = llvm.getStructTy [llvm.getFloatTy 32]
      me.isCompatibleWith(structTy).should.be.false

      fnTy = llvm.getFunctionTy llvm.getVoidTy(), llvm.getFloatTy 32
      me.isCompatibleWith(fnTy).should.be.false

describe 'FunctionType', ->
  describe 'toString', ->
    it 'returns void and parens for thunk', ->
      me = llvm.getFunctionTy()
      me.toString().should.equal 'void ()'

    it 'shows returns value before the parens', ->
      me = llvm.getFunctionTy llvm.getIntTy 32
      me.toString().should.equal 'i32 ()'

    it 'shows parameter values within the parens', ->
      me = llvm.getFunctionTy llvm.getVoidTy(),
        llvm.getIntTy 32
        llvm.getIntTy 1
      me.toString().should.equal 'void (i32, i1)'

    it 'shows nested function types', ->
      source = llvm.getFunctionTy llvm.getIntTy 64
      sink = llvm.getFunctionTy llvm.getVoidTy(), llvm.getIntTy 64
      me = llvm.getFunctionTy llvm.getFunctionTy(), source, sink
      me.toString().should.equal 'void () (i64 (), void (i64))'

  describe 'isCompatibleWith', ->
    it 'returns true for identical type', ->
      me = llvm.getFunctionTy llvm.getVoidTy(), llvm.getIntTy 32
      other = llvm.getFunctionTy llvm.getVoidTy(), llvm.getIntTy 32
      me.isCompatibleWith(other).should.be.true

    it 'returns false for non-function types', ->
      me = llvm.getFunctionTy llvm.getVoidTy(), llvm.getIntTy 32

      voidTy = llvm.getVoidTy()
      me.isCompatibleWith(voidTy).should.be.false

      intTy = llvm.getIntTy 32
      me.isCompatibleWith(intTy).should.be.false

      floatTy = llvm.getFloatTy 32
      me.isCompatibleWith(floatTy).should.be.false

      ptrTy = llvm.getPointerTy llvm.getIntTy 32
      me.isCompatibleWith(ptrTy).should.be.false

      arrTy = llvm.getArrayTy 3, llvm.getFloatTy 32
      me.isCompatibleWith(arrTy).should.be.false

      structTy = llvm.getStructTy [llvm.getFloatTy 32]
      me.isCompatibleWith(structTy).should.be.false

    it 'returns false for other return types', ->
      me = llvm.getFunctionTy llvm.getVoidTy(), llvm.getIntTy 32
      other = llvm.getFunctionTy llvm.getIntTy(32), llvm.getIntTy 32
      me.isCompatibleWith(other).should.be.false

    it 'returns false for other parameter types', ->
      me = llvm.getFunctionTy llvm.getVoidTy(), llvm.getIntTy 32
      bigger = llvm.getFunctionTy llvm.getVoidTy(), llvm.getIntTy 64
      me.isCompatibleWith(bigger).should.be.false

      additional = llvm.getFunctionTy llvm.getVoidTy(), llvm.getIntTy(32), llvm.getIntTy(32)
      me.isCompatibleWith(additional).should.be.false

describe 'IntType', ->
  describe 'toString', ->
    it 'returns int and bit width', ->
      me = llvm.getIntTy 32
      me.toString().should.equal 'i32'

  describe 'constructor', ->
    it 'errors on zero bit width', ->
      (-> llvm.getIntTy 0).should.throw /bit width/i

    it 'errors on partial bit width', ->
      (-> llvm.getIntTy 1.5).should.throw /bit width/i

    it 'errors on negative bit width', ->
      (-> llvm.getIntTy -32).should.throw /bit width/i

    it 'errors on gargantuan bit width', ->
      (-> llvm.getIntTy 99999999).should.throw /bit width/i

  describe 'isCompatibleWith', ->
    it 'returns true for identical type', ->
      me = llvm.getIntTy 32
      other = llvm.getIntTy 32
      me.isCompatibleWith(other).should.be.true

    it 'returns false for non-integer types', ->
      me = llvm.getIntTy 32

      voidTy = llvm.getVoidTy()
      me.isCompatibleWith(voidTy).should.be.false

      floatTy = llvm.getFloatTy 32
      me.isCompatibleWith(floatTy).should.be.false

      ptrTy = llvm.getPointerTy llvm.getIntTy 32
      me.isCompatibleWith(ptrTy).should.be.false

      arrTy = llvm.getArrayTy 3, llvm.getIntTy 32
      me.isCompatibleWith(arrTy).should.be.false

      structTy = llvm.getStructTy [llvm.getFloatTy 32]
      me.isCompatibleWith(structTy).should.be.false

      fnTy = llvm.getFunctionTy llvm.getVoidTy(), llvm.getFloatTy 32
      me.isCompatibleWith(fnTy).should.be.false

    it 'returns false for other bitwidth types', ->
      me = llvm.getIntTy 32

      smaller = llvm.getIntTy 16
      me.isCompatibleWith(smaller).should.be.false

      larger = llvm.getIntTy 64
      me.isCompatibleWith(larger).should.be.false

describe 'FloatType', ->
  describe 'toString', ->
    it 'returns float type', ->
      me = llvm.getFloatTy 32
      me.toString().should.equal 'float'

    it 'returns double type', ->
      me = llvm.getFloatTy 64
      me.toString().should.equal 'double'

    it 'returns half type', ->
      me = llvm.getFloatTy 16
      me.toString().should.equal 'half'

  describe 'constructor', ->
    it 'errors on zero bit width', ->
      (-> llvm.getFloatTy 0).should.throw /bit width/i

    it 'errors on partial bit width', ->
      (-> llvm.getFloatTy 1.5).should.throw /bit width/i

    it 'errors on negative bit width', ->
      (-> llvm.getFloatTy -32).should.throw /bit width/i

    it 'errors on gargantuan bit width', ->
      (-> llvm.getFloatTy 99999999).should.throw /bit width/i

    it 'errors on non-standard bit width', ->
      (-> llvm.getFloatTy 15).should.throw /bit width/i
      (-> llvm.getFloatTy 17).should.throw /bit width/i
      (-> llvm.getFloatTy 31).should.throw /bit width/i
      (-> llvm.getFloatTy 33).should.throw /bit width/i
      (-> llvm.getFloatTy 63).should.throw /bit width/i
      (-> llvm.getFloatTy 65).should.throw /bit width/i

    it 'creates halfs, floats, and doubles', ->
      (-> llvm.getFloatTy 16).should.not.throw /bit width/i
      (-> llvm.getFloatTy 32).should.not.throw /bit width/i
      (-> llvm.getFloatTy 64).should.not.throw /bit width/i

  describe 'isCompatibleWith', ->
    it 'returns true for identical type', ->
      me = llvm.getFloatTy 32
      other = llvm.getFloatTy 32
      me.isCompatibleWith(other).should.be.true

    it 'returns false for non-float types', ->
      me = llvm.getFloatTy 32

      voidTy = llvm.getVoidTy()
      me.isCompatibleWith(voidTy).should.be.false

      intTy = llvm.getIntTy 32
      me.isCompatibleWith(intTy).should.be.false

      ptrTy = llvm.getPointerTy llvm.getIntTy 32
      me.isCompatibleWith(ptrTy).should.be.false

      arrTy = llvm.getArrayTy 3, llvm.getIntTy 32
      me.isCompatibleWith(arrTy).should.be.false

      structTy = llvm.getStructTy [llvm.getFloatTy 32]
      me.isCompatibleWith(structTy).should.be.false

      fnTy = llvm.getFunctionTy llvm.getVoidTy(), llvm.getFloatTy 32
      me.isCompatibleWith(fnTy).should.be.false

    it 'returns false for other bitwidth types', ->
      me = llvm.getFloatTy 32

      smaller = llvm.getFloatTy 16
      me.isCompatibleWith(smaller).should.be.false

      larger = llvm.getFloatTy 64
      me.isCompatibleWith(larger).should.be.false

describe 'PointerType', ->
  describe 'toString', ->
    it 'returns the pointee type and an asterisk', ->
      pointee = llvm.getIntTy 32
      me = llvm.getPointerTy pointee
      me.toString().should.equal 'i32*'

  describe 'isCompatibleWith', ->
    it 'returns true for identical type', ->
      me = llvm.getPointerTy llvm.getIntTy 32
      other = llvm.getPointerTy llvm.getIntTy 32
      me.isCompatibleWith(other).should.be.true

    it 'returns false for non-pointer types', ->
      me = llvm.getPointerTy llvm.getIntTy 32

      voidTy = llvm.getVoidTy()
      me.isCompatibleWith(voidTy).should.be.false

      intTy = llvm.getIntTy 32
      me.isCompatibleWith(intTy).should.be.false

      floatTy = llvm.getFloatTy 32
      me.isCompatibleWith(floatTy).should.be.false

      arrTy = llvm.getArrayTy 3, llvm.getIntTy 32
      me.isCompatibleWith(arrTy).should.be.false

      structTy = llvm.getStructTy [llvm.getFloatTy 32]
      me.isCompatibleWith(structTy).should.be.false

      fnTy = llvm.getFunctionTy llvm.getVoidTy(), llvm.getFloatTy 32
      me.isCompatibleWith(fnTy).should.be.false

    it 'returns false for other pointee types', ->
      me = llvm.getPointerTy llvm.getIntTy 32

      smaller = llvm.getPointerTy llvm.getIntTy 16
      me.isCompatibleWith(smaller).should.be.false

      larger = llvm.getPointerTy llvm.getIntTy 64
      me.isCompatibleWith(larger).should.be.false

describe 'ArrayType', ->
  describe 'toString', ->
    it 'returns the count and element', ->
      me = llvm.getArrayTy 42, llvm.getIntTy 64
      me.toString().should.equal '[42 x i64]'

  describe 'isCompatibleWith', ->
    it 'returns true for identical type', ->
      me = llvm.getArrayTy 3, llvm.getIntTy 32
      other = llvm.getArrayTy 3, llvm.getIntTy 32
      me.isCompatibleWith(other).should.be.true

    it 'returns false for non-array types', ->
      me = llvm.getArrayTy 3, llvm.getIntTy 32

      voidTy = llvm.getVoidTy()
      me.isCompatibleWith(voidTy).should.be.false

      intTy = llvm.getIntTy 32
      me.isCompatibleWith(intTy).should.be.false

      floatTy = llvm.getFloatTy 32
      me.isCompatibleWith(floatTy).should.be.false

      ptrTy = llvm.getPointerTy llvm.getIntTy 32
      me.isCompatibleWith(ptrTy).should.be.false

      structTy = llvm.getStructTy [llvm.getFloatTy 32]
      me.isCompatibleWith(structTy).should.be.false

      fnTy = llvm.getFunctionTy llvm.getVoidTy(), llvm.getFloatTy 32
      me.isCompatibleWith(fnTy).should.be.false

    it 'returns false for other element types', ->
      me = llvm.getArrayTy 3, llvm.getIntTy 32

      smaller = llvm.getArrayTy 3, llvm.getIntTy 16
      me.isCompatibleWith(smaller).should.be.false

      larger = llvm.getArrayTy 3, llvm.getIntTy 64
      me.isCompatibleWith(larger).should.be.false

    it 'returns false for other sizes', ->
      me = llvm.getArrayTy 3, llvm.getIntTy 32

      smaller = llvm.getArrayTy 2, llvm.getIntTy 32
      me.isCompatibleWith(smaller).should.be.false

      larger = llvm.getArrayTy 4, llvm.getIntTy 32
      me.isCompatibleWith(larger).should.be.false

describe 'StructType', ->
  describe 'toString', ->
    i1 = llvm.getIntTy 1
    i8 = llvm.getIntTy 8
    i16 = llvm.getIntTy 16
    i32 = llvm.getIntTy 32

    it 'returns the elements types', ->
      me = llvm.getStructTy [i1, i8, i16, i32]
      me.toString().should.equal '{i1, i8, i16, i32}'

  describe 'isCompatibleWith', ->
    it 'returns true for identical type', ->
      me = llvm.getStructTy [llvm.getIntTy 32]
      other = llvm.getStructTy [llvm.getIntTy 32]
      me.isCompatibleWith(other).should.be.true

    it 'returns false for non-struct types', ->
      me = llvm.getStructTy [llvm.getIntTy 32]

      voidTy = llvm.getVoidTy()
      me.isCompatibleWith(voidTy).should.be.false

      intTy = llvm.getIntTy 32
      me.isCompatibleWith(intTy).should.be.false

      floatTy = llvm.getFloatTy 32
      me.isCompatibleWith(floatTy).should.be.false

      ptrTy = llvm.getPointerTy llvm.getIntTy 32
      me.isCompatibleWith(ptrTy).should.be.false

      arrTy = llvm.getArrayTy 3, llvm.getFloatTy 32
      me.isCompatibleWith(arrTy).should.be.false

      fnTy = llvm.getFunctionTy llvm.getVoidTy(), llvm.getFloatTy 32
      me.isCompatibleWith(fnTy).should.be.false

    it 'returns false for other element types', ->
      me = llvm.getStructTy [llvm.getIntTy 32]

      smaller = llvm.getStructTy [llvm.getIntTy 16]
      me.isCompatibleWith(smaller).should.be.false

      larger = llvm.getStructTy [llvm.getIntTy 64]
      me.isCompatibleWith(larger).should.be.false

      additional = llvm.getStructTy [llvm.getIntTy(32), llvm.getIntTy(32)]
      me.isCompatibleWith(additional).should.be.false
