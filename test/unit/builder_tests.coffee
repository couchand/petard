# test the builder

llvm = require '../../'

vd = llvm.getVoidTy()
i8 = llvm.getIntTy 8
i32 = llvm.getIntTy 32
i64 = llvm.getIntTy 64

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

  describe 'parameter', ->
    it 'expects a parameter index', ->
      me = unit.makeFunction 'nothing'
      (-> me.parameter()).should.throw /index/i
      (-> me.parameter 'foobar').should.throw /index/i

    it 'expects a valid parameter index', ->
      me = unit.makeFunction 'onething', vd, i32
      (-> me.parameter 1).should.throw /index/i

    it 'produces a parameter value', ->
      me = unit.makeFunction 'it', vd, i32
      param = me.parameter 0
      param.type.toString().should.equal 'i32'

  describe 'alloca', ->
    it 'expects a type', ->
      me = unit.makeFunction 'nothing'
      (-> me.alloca()).should.throw /type/i
      (-> me.alloca 42).should.throw /type/i

    it 'produces a single alloca', ->
      me = unit.makeFunction 'something'
      spot = me.alloca i32
      spot.type.toString().should.equal 'i32*'

    it 'produces a range alloca from a number', ->
      me = unit.makeFunction 'something'
      spot = me.alloca i32, 5
      spot.type.toString().should.equal 'i32*'

    it 'produces a range alloca from a value', ->
      me = unit.makeFunction 'something', vd, i32
      param = me.parameter 0
      spot = me.alloca i32, param
      spot.type.toString().should.equal 'i32*'

    it 'expects a numeric type for the range', ->
      me = unit.makeFunction 'something', vd, llvm.getPointerTy i32
      ptr = me.parameter 0
      (-> me.alloca i32, ptr).should.throw /size/i

  describe 'load', ->
    it 'expects a pointer', ->
      me = unit.makeFunction 'something', vd, i32
      (-> me.load()).should.throw /pointer/i
      (-> me.load 42).should.throw /pointer/i
      (-> me.load me.parameter 0).should.throw /pointer/i

    it 'produces a load', ->
      me = unit.makeFunction 'nothing'
      spot = me.alloca i32
      ld = me.load spot
      ld.type.toString().should.equal 'i32'

  describe 'store', ->
    it 'expects a value to store', ->
      me = unit.makeFunction 'something', vd, i32
      (-> me.store()).should.throw /value/i
      (-> me.store "foobar").should.throw /value/i

    it 'expects a pointer', ->
      me = unit.makeFunction 'something', vd, i32
      (-> me.store 42).should.throw /pointer/i
      (-> me.store 42, 42).should.throw /pointer/i
      (-> me.store 42, me.parameter 0).should.throw /pointer/i

    it 'expects pointer type to match value type', ->
      me = unit.makeFunction 'nothing'
      intSpot = me.alloca i32
      bigVal = me.value i64, 42
      (-> me.store bigVal, intSpot).should.throw /type/i
      smallVal = me.value i8, 10
      (-> me.store smallVal, intSpot).should.throw /type/i
      floatVal = me.value llvm.getFloatTy(32), 3.141
      (-> me.store floatVal, intSpot).should.throw /type/i

  describe 'getElementPointer', ->
    me = beforeEach -> me = unit.makeFunction 'nothing'
    it 'expects a base pointer', ->
      (-> me.getElementPointer()).should.throw /pointer/i
      (-> me.getElementPointer 42).should.throw /pointer/i
      intVal = me.value i32, 42
      (-> me.getElementPointer intVal).should.throw /pointer/i

    it 'expects at least one index', ->
      spot = me.alloca i32
      (-> me.getElementPointer spot).should.throw /index/i
      (-> me.getElementPointer spot, []).should.throw /index/i

    it 'accepts a number for an index', ->
      spot = me.alloca i32
      ptr = me.getElementPointer spot, 0
      ptr.type.toString().should.equal 'i32*'

      arrSpot = me.alloca llvm.getArrayTy 3, i32
      arrPtr = me.getElementPointer arrSpot, 0, 0
      arrPtr.type.toString().should.equal 'i32*'

    it 'expects index list to not exceed type depth', ->
      intSpot = me.alloca i32
      (-> me.getElementPointer intSpot, 0, 0).should.throw /index/i

      arrSpot = me.alloca llvm.getArrayTy 3, i32
      (-> me.getElementPointer arrSpot, 0, 0, 0).should.throw /index/i

    it 'accepts a value for an index', ->
      me = unit.makeFunction 'something', vd, i32
      p = me.parameter 0

      spot = me.alloca i32
      ptr = me.getElementPointer spot, p
      ptr.type.toString().should.equal 'i32*'

    it 'expects a constant for a struct', ->
      return
      me = unit.makeFunction 'something', vd, i32
      p = me.parameter 0

      spot = me.alloca llvm.getStructTy [i32]
      (-> me.getElementPointer spot, 0, p).should.throw /constant/i

    it 'indexed into a struct', ->
      spot = me.alloca llvm.getStructTy [i32]
      ptr = me.getElementPointer spot, 0, 0
      ptr.type.toString().should.equal 'i32*'
