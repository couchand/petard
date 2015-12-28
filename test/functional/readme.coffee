# tests from the README

fs = require 'fs'

{chai, should} = require '../helper'

llvm = require '../../'

describe 'README', ->
  describe 'getting started', ->
    it 'illustrates basic usage', ->
      hello = llvm.CodeUnit "hello"

      main = hello.makeFunction "main", llvm.getFunctionTy llvm.type.i32
      puts = hello.declareFunction "puts", llvm.getFunctionTy llvm.type.i32, llvm.type.pointerTo llvm.type.i8

      text = hello.constant "Hello, world!\n"

      message = main.loadConstant text
      main.callFunction puts, message

      main.return 0

      hello.writeBitcodeToFile "hello.bc"

      fs.statSync "hello.bc"
        .should.not.be.null

      fs.readFileSync "hello.bc"
        .should.have.length.greaterThan 0
