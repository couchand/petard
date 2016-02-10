# test the builder

llvm = require '../../'

{i1, i8, i32, i64, f32, pointerTo, structOf, vectorOf, arrayOf} = llvm.type
vd = llvm.getVoidTy()
i8p = pointerTo i8
i32p = pointerTo i32

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
      me = unit.makeFunction 'pointer', i8p
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
      me = unit.makeFunction 'something', vd, i32p
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
      floatVal = me.value f32, 3.141
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
      me = unit.makeFunction 'something', vd, i32
      p = me.parameter 0

      spot = me.alloca llvm.getStructTy [i32]
      (-> me.getElementPointer spot, 0, p).should.throw /index/i

    it 'indexes into a struct', ->
      spot = me.alloca llvm.getStructTy [i32]
      ptr = me.getElementPointer spot, 0, 0
      ptr.type.toString().should.equal 'i32*'

    it 'indexes into a vector', ->
      spot = me.alloca llvm.getVectorTy 3, i32
      ptr = me.getElementPointer spot, 0, 0
      ptr.type.toString().should.equal 'i32*'

    it 'indexes with a vector', ->
      vt = llvm.getVectorTy 3, i32
      me = unit.makeFunction 'vecfn', i32, vt
      spot = me.alloca llvm.getArrayTy 3, i32
      ptrs = me.getElementPointer spot, 0, me.parameter 0
      ptrs.type.toString().should.equal '<3 x i32*>'

    it 'indexes from a vector', ->
      vt = llvm.getVectorTy 3, llvm.getPointerTy llvm.getArrayTy 3, i32
      me = unit.makeFunction 'vecfn', i32, vt
      ptrs = me.getElementPointer me.parameter(0), 0, 0
      ptrs.type.toString().should.equal '<3 x i32*>'

    it 'index from a vector with a vector', ->
      vt = llvm.getVectorTy 3, llvm.getPointerTy llvm.getArrayTy 3, i32
      it = llvm.getVectorTy 3, i32
      me = unit.makeFunction 'vecfn', i32, vt, it
      bases = me.parameter 0
      idxs = me.parameter 1
      ptrs = me.getElementPointer me.parameter(0), 0, idxs
      ptrs.type.toString().should.equal '<3 x i32*>'

  describe 'callFunction', ->
    f = g = me = beforeEach ->
      f = unit.makeFunction 'f', i32, i32
      g = unit.declareFunction 'g', i32, i32
      me = unit.makeFunction 'main', i32

    it 'expects a function to call', ->
      (-> me.callFunction()).should.throw /function/i
      (-> me.callFunction 42).should.throw /function/i
      (-> me.callFunction me.value i32, 0).should.throw /function/i

    it 'can call a functionbuilder', ->
      call = me.callFunction f, me.value i32, 0
      call.type.toString().should.equal 'i32'

    it 'can call a functionvalue', ->
      call = me.callFunction g, me.value i32, 0
      call.type.toString().should.equal 'i32'

    it 'expects the parameters for the function type', ->
      (-> me.callFunction f).should.throw /parameter/i
      (-> me.callFunction f, me.value i8, 0).should.throw /parameter/i
      (-> me.callFunction g).should.throw /parameter/i
      (-> me.callFunction g, me.value i8, 0).should.throw /parameter/i

  describe 'select', ->
    me = truth = beforeEach ->
      me = unit.makeFunction 'main', i32
      truth = me.value i1, 1

    it 'expects a condition', ->
      (-> me.select()).should.throw /condition/i
      (-> me.select 42).should.throw /condition/i
      (-> me.select me.value i32, 0).should.throw /condition/i

    it 'expects an if true value', ->
      (-> me.select truth).should.throw /ifTrue/i

    it 'expects an if false value', ->
      one = me.value i32, 1
      (-> me.select truth, one).should.throw /ifFalse/i

    it 'expects the two values to have the same type', ->
      one = me.value i32, 1
      two = me.value f32, 2
      (-> me.select truth, one, two).should.throw /type/i

    it 'produces a select instruction', ->
      one = me.value i32, 1
      two = me.value i32, 2
      sel = me.select truth, one, two
      sel.type.toString().should.equal 'i32'

    it 'operates on vectors', ->
      vty = llvm.getVectorTy 3, f32
      chty = llvm.getVectorTy 3, i1
      vecsel = unit.makeFunction 'vecsel', vty, vty, vty, chty
      l = vecsel.parameter 0
      r = vecsel.parameter 1
      ch = vecsel.parameter 2
      selected = vecsel.select ch, l, r
      selected.type.toString().should.equal vty.toString()

  describe 'value', ->
    me = beforeEach -> me = unit.makeFunction 'main', i32, i32

    it 'expects a type', ->
      (-> me.value()).should.throw /type/i
      (-> me.value 42).should.throw /type/i
      (-> me.value me.parameter 0).should.throw /type/i

    it 'expects a constant value', ->
      (-> me.value i32).should.throw /value/i
      (-> me.value i32, me.parameter 0).should.throw /value/i

    it 'produces integer values', ->
      val = me.value i32, 42
      val.type.toString().should.equal 'i32'

    it 'produces floating point values', ->
      val = me.value f32, 3.141
      val.type.toString().should.equal 'float'

  describe 'extractElement', ->
    me = beforeEach -> me = unit.makeFunction 'main', i32, i32, llvm.getVectorTy 3, i32

    it 'expects a vector value', ->
      (-> me.extractElement()).should.throw /vector/i
      (-> me.extractElement 42).should.throw /vector/i
      (-> me.extractElement me.parameter 0).should.throw /vector/i

    it 'expects an integer index', ->
      v = me.parameter 1
      (-> me.extractElement v).should.throw /index/i
      (-> me.extractElement v, me.parameter 1).should.throw /index/i

    it 'produces an extractelement instruction', ->
      v = me.parameter 1
      n = me.extractElement v, 0
      n.type.toString().should.equal 'i32'

  describe 'insertElement', ->
    me = beforeEach -> me = unit.makeFunction 'main', i32, i32, llvm.getVectorTy 3, i32

    it 'expects a vector value', ->
      (-> me.insertElement()).should.throw /vector/i
      (-> me.insertElement 42).should.throw /vector/i
      (-> me.insertElement me.parameter 0).should.throw /vector/i

    it 'expects a value to insert', ->
      v = me.parameter 1
      (-> me.insertElement v).should.throw /value/i

    it 'expects an integer index', ->
      v = me.parameter 1
      (-> me.insertElement v, me.parameter 0).should.throw /index/i

    it 'produces an insertelement instruction', ->
      x = me.parameter 0
      v = me.parameter 1
      n = me.insertElement v, x, 0
      n.type.toString().should.equal v.type.toString()

  describe 'binary arithmetic', ->
    me = beforeEach -> me = unit.makeFunction 'main', i32, i32, f32, vectorOf(3, i32), vectorOf(3, f32), vectorOf(4, f32)

    testBinary = (name, acceptsInts, acceptsFloats, returns) ->
      describe name, ->
        it 'expects two values', ->
          (-> me[name]()).should.throw /value/i
          (-> me[name] 42).should.throw /value/i
          (-> me[name] me.parameter 0).should.throw /value/i

        it 'expects numeric values', ->
          me = unit.makeFunction 'variety', i32, f32, i8, i8p, structOf([i32]), arrayOf(3, i32)

          float = me.parameter 0
          char = me.parameter 1
          charp = me.parameter 2
          struct = me.parameter 3
          array = me.parameter 4

          (-> me[name] float, float).should.not.throw() if acceptsFloats
          (-> me[name] char, char).should.not.throw() if acceptsInts

          (-> me[name] charp, charp).should.throw /value/i
          (-> me[name] struct, struct).should.throw /value/i
          (-> me[name] array, array).should.throw /value/i

        if acceptsInts
          it 'accepts ints', ->
            int = me.parameter 0
            result = me[name] int, int
            result.type.toString().should.equal if returns then returns.toString() else int.type.toString()

          it 'accepts int vectors', ->
            vec = me.parameter 2
            result = me[name] vec, vec
            result.type.toString().should.equal if returns then returns.toString() else vec.type.toString()
        else
          it 'does not accept ints', ->
            int = me.parameter 0
            (-> me[name] int, int).should.throw /type/i

          it 'does not accept int vectors', ->
            vec = me.parameter 2
            (-> me[name] vec, vec).should.throw /type/i

        if acceptsFloats
          it 'accepts floats', ->
            flo = me.parameter 1
            result = me[name] flo, flo
            result.type.toString().should.equal if returns then returns.toString() else flo.type.toString()

          it 'accepts float vectors', ->
            vec = me.parameter 3
            result = me[name] vec, vec
            result.type.toString().should.equal if returns then returns.toString() else vec.type.toString()
        else
          it 'does not accept floats', ->
            float = me.parameter 1
            (-> me[name] float, float).should.throw /type/i

          it 'does not accept float vectors', ->
            vec = me.parameter 3
            (-> me[name] vec, vec).should.throw /type/i

        it 'expects vectors to be the same length', ->
          vec3 = me.parameter 3
          vec4 = me.parameter 4
          (-> me[name] vec3, vec4).should.throw /length/i

        it 'expects types to be the same', ->
          i = me.parameter 0
          f = me.parameter 1
          (-> me[name] i, f).should.throw /type/i

        it 'expects vector element types to be the same', ->
          iv = me.parameter 2
          fv = me.parameter 3
          (-> me[name] iv, fv).should.throw /type/i

        it 'expects the vector element to be numeric', ->
          me = unit.makeFunction 'variety', i32, vectorOf(3, i32p)
          pv = me.parameter 0
          (-> me[name] pv, pv).should.throw /type/i

    testBinary 'add', yes, yes
    testBinary 'sub', yes, yes
    testBinary 'mul', yes, yes
    testBinary 'udiv', yes, no
    testBinary 'sdiv', yes, no
    testBinary 'fdiv', no, yes
    testBinary 'urem', yes, no
    testBinary 'srem', yes, no
    testBinary 'frem', no, yes
    testBinary 'and', yes, no
    testBinary 'or', yes, no
    testBinary 'xor', yes, no
    testBinary 'shl', yes, no
    testBinary 'lshr', yes, no
    testBinary 'ashr', yes, no

    testBinary 'equal', yes, no, i1
    testBinary 'notEqual', yes, no, i1
    testBinary 'uGreaterThan', yes, no, i1
    testBinary 'uAtLeast', yes, no, i1
    testBinary 'uLessThan', yes, no, i1
    testBinary 'uAtMost', yes, no, i1
    testBinary 'sGreaterThan', yes, no, i1
    testBinary 'sAtLeast', yes, no, i1
    testBinary 'sLessThan', yes, no, i1
    testBinary 'sAtMost', yes, no, i1
    testBinary 'foEqual', no, yes, i1
    testBinary 'foNotEqual', no, yes, i1
    testBinary 'foGreaterThan', no, yes, i1
    testBinary 'foAtLeast', no, yes, i1
    testBinary 'foLessThan', no, yes, i1
    testBinary 'foAtMost', no, yes, i1
    testBinary 'fuEqual', no, yes, i1
    testBinary 'fuNotEqual', no, yes, i1
    testBinary 'fuGreaterThan', no, yes, i1
    testBinary 'fuAtLeast', no, yes, i1
    testBinary 'fuLessThan', no, yes, i1
    testBinary 'fuAtMost', no, yes, i1
