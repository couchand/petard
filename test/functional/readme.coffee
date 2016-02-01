# tests from the README

fs = require 'fs'
tmp = require 'tmp'

{chai, should} = require '../helper'

llvm = require '../../'

describe 'README', ->
  describe 'getting started', ->
    it 'illustrates basic usage', ->
      hello = llvm.CodeUnit "hello"

      main = hello.makeFunction "main", llvm.type.i32
      puts = hello.declareFunction "puts", llvm.type.i32, llvm.type.pointerTo llvm.type.i8

      text = hello.constant "Hello, world!\n"

      message = main.loadConstant text
      main.callFunction puts, message

      main.return 0

      filename = tmp.tmpNameSync()

      hello.writeBitcodeToFile filename

      fs.statSync filename
        .should.not.be.null

      fs.readFileSync filename
        .should.have.length.greaterThan 0

      fs.unlink filename
