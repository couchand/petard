# fib test

TARGET_INDEX = 10
EXPECTED_VALUE = 89

fs = require 'fs'
tmp = require 'tmp'

{chai, should} = require '../helper'

llvm = require '../../'

{i1, i8, i32, pointerTo} = llvm.type
vd = llvm.type.void

describe 'conditionals', ->
  it 'illustrates basic usage', ->
    
    mod = llvm.CodeUnit "fib"
    
    main = mod.makeFunction "main", i32
    fib = mod.makeFunction "fib", i32, i32
    
    zero = main.value i32, 0
    one = main.value i32, 1
    two = main.value i32, 2
    
    p = fib.parameter 0
    
    ifHi = fib.if fib.equal zero, p
    ifHi.then.return one
    
    nested = ifHi.else.if ifHi.else.equal one, p
    nested.then.return one
    
    lessOne = nested.else.callFunction fib, nested.else.sub p, one
    lessTwo = nested.else.callFunction fib, nested.else.sub p, two
    
    nested.else.return nested.else.add lessOne, lessTwo
    
    main.return main.callFunction fib, main.value i32, TARGET_INDEX
    
    mainf = main.jitCompile()

    fibval = mainf()

    fibval.should.equal EXPECTED_VALUE
