# test the builder

llvm = require '../../'

{should} = require '../helper'

describe 'CodeUnit', ->
  describe 'constructor', ->
    it 'creates a code unit', ->
      me = new llvm.CodeUnit 'foobar.baz'
      me.should.be.an.instanceof llvm.CodeUnit

    it 'doesn\'t require new', ->
      me = llvm.CodeUnit 'foobar.baz'
      me.should.be.an.instanceof llvm.CodeUnit

  describe 'makeFunction', ->
    it 'is a function', ->
      llvm.CodeUnit.should.respondTo 'makeFunction'

    it 'creates a function builder', ->
      me = new llvm.CodeUnit 'foobar.baz'

      fn = me.makeFunction 'doIt', llvm.getFunctionTy()

      fn.constructor.name.should.equal 'FunctionBuilder'

describe 'FunctionBuilder', ->
  unit = beforeEach -> unit = new llvm.CodeUnit 'foobar.baz'

  describe 'name', ->
    it 'returns the function name', ->
      me = unit.makeFunction 'myName', llvm.getFunctionTy()
      me.name.should.equal 'myName'

  describe 'type', ->
    it 'returns the function type', ->
      me = unit.makeFunction 'something', llvm.getFunctionTy llvm.getIntTy(1), llvm.getIntTy(2)
      me.type.toString().should.equal 'i1 (i2)'
