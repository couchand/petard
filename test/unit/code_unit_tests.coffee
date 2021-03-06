# test the code unit

llvm = require '../../'

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

      fn = me.makeFunction 'doIt'

      fn.constructor.name.should.equal 'FunctionBuilder'

    it 'takes parameters for return and argument types', ->
      me = new llvm.CodeUnit 'foobar.baz'

      fn = me.makeFunction 'something', llvm.getIntTy(1), llvm.getIntTy(2)

      fn.type.toString().should.equal 'i1 (i2)'

  describe 'declareFunction', ->
    it 'is a function', ->
      llvm.CodeUnit.should.respondTo 'declareFunction'

    it 'declares a function', ->
      me = new llvm.CodeUnit 'foobar.baz'

      fn = me.declareFunction 'doIt'

      fn.constructor.name.should.equal 'Value'

    it 'takes parameters for return and argument types', ->
      me = new llvm.CodeUnit 'foobar.baz'

      fn = me.declareFunction 'something', llvm.getIntTy(1), llvm.getIntTy(2)

      fn.type.toString().should.equal 'i1 (i2)'

  describe 'constant', ->
    it 'creates string constants', ->
      unit = new llvm.CodeUnit 'constant.test'

      foobar = unit.constant 'foobar'

      foobar.type.toString().should.equal '[7 x i8]*'
