# tests from the README

fs = require 'fs'

{chai, should} = require '../helper'

llvm = require '../../'

describe 'README', ->
  mainTy = llvm.getFunctionTy llvm.type.i32
  putsTy = llvm.getFunctionTy llvm.type.i32, llvm.type.pointerTo llvm.type.i8

  describe 'getting started', ->
    it 'illustrates basic usage', ->
      hello = llvm.CodeUnit "hello"

      main = hello.makeFunction "main", mainTy
      puts = hello.declareFunction "puts", putsTy

      text = hello.constant "Hello, world!\n"

      message = main.loadConstant text
      main.callFunction puts, message

      main.return 0

      hello.writeBitcodeToFile "hello.bc"

      fs.statSync "hello.bc"
        .should.not.be.null

      fs.readFileSync "hello.bc"
        .should.have.length.greaterThan 0
