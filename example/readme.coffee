# README example (see also the test version)
# try running the output of this script through clang

llvm = require '../'

hello = llvm.CodeUnit "hello"

main = hello.makeFunction "main", llvm.type.i32
puts = hello.declareFunction "puts", llvm.type.i32, llvm.type.pointerTo llvm.type.i8

text = hello.constant "Hello, world!\n"

message = main.loadConstant text
main.callFunction puts, message

main.return 0

hello.writeBitcodeToFile "hello.bc"
hello.dump()
