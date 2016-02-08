# foreign-function interface with jit compiled code

llvm = require '../'

module = llvm.CodeUnit "munge"

munge = module.makeFunction "munge", llvm.type.i32, llvm.type.i32, llvm.type.i32

a = munge.parameter 0
b = munge.parameter 1

sum = munge.add a, b
product = munge.mul a, b

munge.return munge.sub product, sum

module.dump()

x = 3
y = 4

console.log "munging", x, "and", y

f = munge.jitCompile 'int', ['int', 'int']

console.log "=", f x, y

# expecting 5
