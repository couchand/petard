# test the builder

llvm = require '../../'

i8 = llvm.getIntTy 8
i32 = llvm.getIntTy 32

describe 'FunctionBuilder', ->
  unit = beforeEach -> unit = new llvm.CodeUnit 'foobar.baz'

  describe 'name', ->
    it 'returns the function name', ->
      me = unit.makeFunction 'myName'
      me.name.should.equal 'myName'

  describe 'type', ->
    it 'returns the function type', ->
      me = unit.makeFunction 'something', llvm.getIntTy(1), llvm.getIntTy(2)
      me.type.toString().should.equal 'i1 (i2)'

  describe 'return', ->
    it 'produces a return void', ->
      me = unit.makeFunction 'nothing'
      me.return()

    it 'rejects a value if return type is void', ->
      me = unit.makeFunction 'reallyNothing'
      (-> me.return 42).should.throw /type/i

    it 'accepts a number if it can convert', ->
      me = unit.makeFunction 'something', i32
      me.return 42

    it 'rejects a number if unconvertible', ->
      me = unit.makeFunction 'pointer', llvm.getPointerTy i8
      (-> me.return 42).should.throw /type/i

    it 'accepts a value', ->
      me = unit.makeFunction 'number', i32
      answer = me.value i32, 42
      me.return answer

    it 'rejects a value if type is wrong', ->
      me = unit.makeFunction 'getPointer', i8
      answer = me.value i32, 42
      (-> me.return answer).should.throw /type/i
