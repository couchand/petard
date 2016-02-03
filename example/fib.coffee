# fibonacci numbers

TARGET_INDEX = 10

llvm = require '../'

{i1, i8, i32, pointerTo} = llvm.type
vd = llvm.type.void

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

# required because of my poor design
# dead code elimination needs a well-formed function
# so even if it's not possible to reach here, since we
# have the blocks aready we have to terminate them.
fib.return one

main.return main.callFunction fib, main.value i32, TARGET_INDEX

mod.writeBitcodeToFile "fib.bc"
mod.dump()
