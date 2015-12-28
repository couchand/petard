# README example (see also the test version)
# try running the output of this script through clang

llvm = require '../'

mainTy = llvm.getFunctionTy llvm.type.i32
putsTy = llvm.getFunctionTy llvm.type.i32, llvm.type.pointerTo llvm.type.i8

hello = llvm.CodeUnit "hello"

main = hello.makeFunction "main", mainTy
puts = hello.declareFunction "puts", putsTy

text = hello.constant "Hello, world!\n"

message = main.loadConstant text
main.callFunction puts, message

main.return 0

hello.writeBitcodeToFile "hello.bc"
hello.dump()
