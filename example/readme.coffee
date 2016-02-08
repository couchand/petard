# README example (see also the test version)
# try running the output of this script through clang

llvm = require '../'

{i8, i32, pointerTo} = llvm.type

hello = llvm.CodeUnit "hello"

main = hello.makeFunction "main", i32
puts = hello.declareFunction "puts", i32, pointerTo i8

text = hello.constant "Hello, world!\n"

message = main.loadConstant text
main.callFunction puts, message

main.return 0

hello.writeBitcodeToFile "readme.bc"
hello.dump()

hi = hello.makeFunction "hi", i32, pointerTo i8

prefix = hello.constant "Hello, "

hi.callFunction puts, hi.loadConstant prefix
hi.callFunction puts, hi.parameter 0

hi.return 0

greet = hi.jitCompile()

greet "Bill"
greet "Janice"
greet "Bart"
