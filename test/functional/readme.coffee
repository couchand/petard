# tests from the README

{chai, should} = require '../helper'

llvm = require '../../'

describe 'README', ->
  int32 = llvm.getIntTy 32
  int8 = llvm.getIntTy 8

  mainTy = llvm.getFunctionTy int32
  putsTy = llvm.getFunctionTy int32, llvm.getPointerTy int8

  describe 'getting started', ->
    it 'illustrates basic usage', ->
      hello = llvm.CodeUnit "hello"

      main = hello.makeFunction "main", mainTy
      puts = hello.declareFunction "puts", putsTy

      text = hello.constant "Hello, world!\n"

      message = main.loadConstant text
      main.callFunction puts, message

      main.return 0

      #hello.writeBitcodeToFile "hello.bc"
      hello.dump()
