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
