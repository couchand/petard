# test the types

llvm = require '../../'

{should} = require '../helper'

describe 'getters', ->
  describe 'getVoidTy()', ->
    it 'returns a type', ->
      me = llvm.getVoidTy()
      me.constructor.name.should.equal 'Type'

  describe 'getFunctionTy()', ->
    it 'returns a type', ->
      me = llvm.getFunctionTy()
      me.constructor.name.should.equal 'Type'

describe 'VoidType', ->
  describe 'toString()', ->
    it 'returns void', ->
      me = llvm.getVoidTy()
      me.toString().should.equal 'void'

describe 'FunctionType', ->
  describe 'toString()', ->
    it 'returns empty arrow for thunk', ->
      me = llvm.getFunctionTy()
      me.toString().should.equal '->'
