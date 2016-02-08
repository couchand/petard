# function calls and stack allocation

llvm = require '../'

module = llvm.CodeUnit "hello"

noop = module.makeFunction "noop", llvm.type.i32, llvm.type.i32
a = noop.parameter 0
s = noop.alloca llvm.type.i32
noop.store a, s
noop.return noop.load s

main = module.makeFunction "main", llvm.type.i32
theAnswer = main.value llvm.type.i32, 42
main.return main.callFunction noop, theAnswer

module.writeBitcodeToFile 'noop.bc'
