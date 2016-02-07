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

describe 'FunctionType', ->
  describe 'toString', ->
    it 'returns void and parens for thunk', ->
      me = llvm.getFunctionTy()
      me.toString().should.equal 'void ()'

    it 'shows return value before the parens', ->
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

describe 'PointerType', ->
  describe 'toString', ->
    it 'returns the pointee type and an asterisk', ->
      pointee = llvm.getIntTy 32
      me = llvm.getPointerTy pointee
      me.toString().should.equal 'i32*'

describe 'ArrayType', ->
  describe 'toString', ->
    it 'returns the count and element', ->
      me = llvm.getArrayTy 42, llvm.getIntTy 64
      me.toString().should.equal '[42 x i64]'

describe 'StructType', ->
  describe 'toString', ->
    i1 = llvm.getIntTy 1
    i8 = llvm.getIntTy 8
    i16 = llvm.getIntTy 16
    i32 = llvm.getIntTy 32

    it 'returns the elements types', ->
      me = llvm.getStructTy [i1, i8, i16, i32]
      me.toString().should.equal '{i1, i8, i16, i32}'
