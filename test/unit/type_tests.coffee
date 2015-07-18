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

  describe 'getPointerTy', ->
    it 'returns a type', ->
      me = llvm.getPointerTy llvm.getIntTy 32
      me.constructor.name.should.equal 'Type'

    it 'expects a pointee type', ->
      (-> llvm.getPointerTy()).should.throw /pointee/i

describe 'VoidType', ->
  describe 'toString', ->
    it 'returns void', ->
      me = llvm.getVoidTy()
      me.toString().should.equal 'void'

describe 'FunctionType', ->
  describe 'toString', ->
    it 'returns empty arrow for thunk', ->
      me = llvm.getFunctionTy()
      me.toString().should.equal '->'

    it 'shows return value to the right of the arrow', ->
      me = llvm.getFunctionTy llvm.getIntTy 32
      me.toString().should.equal '-> int32'

    it 'shows parameter values to the left of the arrow', ->
      me = llvm.getFunctionTy llvm.getVoidTy(),
        llvm.getIntTy 32
        llvm.getIntTy 1
      me.toString().should.equal 'int32 int1 ->'

    it 'shows nested function types in parens', ->
      source = llvm.getFunctionTy llvm.getIntTy 64
      sink = llvm.getFunctionTy llvm.getVoidTy(), llvm.getIntTy 64
      me = llvm.getFunctionTy llvm.getFunctionTy(), source, sink
      me.toString().should.equal '(-> int64) (int64 ->) -> (->)'

describe 'IntType', ->
  describe 'toString', ->
    it 'returns int and bit width', ->
      me = llvm.getIntTy 32
      me.toString().should.equal 'int32'

    it 'errors on zero bit width', ->
      (-> llvm.getIntTy 0).should.throw /bit width/i

    it 'errors on partial bit width', ->
      (-> llvm.getIntTy 1.5).should.throw /bit width/i

    it 'errors on negative bit width', ->
      (-> llvm.getIntTy -32).should.throw /bit width/i

    it 'errors on gargantuan bit width', ->
      (-> llvm.getIntTy 99999999).should.throw /bit width/i

describe 'PointerType', ->
  describe 'toString', ->
    it 'returns the pointee type and an asterisk', ->
      pointee = llvm.getIntTy 32
      me = llvm.getPointerTy pointee
      me.toString().should.equal 'int32*'
