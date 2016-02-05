# fibonacci numbers

# expected output:
#   the fibonacci numbers up to 832040, one per line

MAX_FIB = 29

llvm = require '../'

{i1, i8, i32, pointerTo} = llvm.type
vd = llvm.type.void

mod = llvm.CodeUnit "fib"

putchar = mod.declareFunction "putchar", vd, i32
putOnes = mod.makeFunction "putOnes", vd, i32
putTens = mod.makeFunction "putTens", vd, i32
puti = mod.makeFunction "puti", vd, i32

newline = mod.makeFunction "newline"
newline.callFunction putchar, newline.value(i32, 10)
newline.return()

onesCh = putOnes.add putOnes.value(i32, 48),
  putOnes.urem putOnes.parameter(0), putOnes.value(i32, 10)
putOnes.callFunction putchar, onesCh
putOnes.return()

divTen = putTens.udiv putTens.parameter(0), putTens.value(i32, 10)
hasTens = putTens.if putTens.equal putTens.value(i32, 0), divTen
hasTens.then.callFunction putchar, putTens.value(i32, 32)
hasTens.else.callFunction putOnes, divTen
putTens.return()

puti.callFunction putTens, puti.udiv puti.parameter(0),
  puti.value(i32, 10000)
puti.callFunction putTens, puti.udiv puti.parameter(0),
  puti.value(i32, 1000)
puti.callFunction putTens, puti.udiv puti.parameter(0),
  puti.value(i32, 100)
puti.callFunction putTens, puti.udiv puti.parameter(0),
  puti.value(i32, 10)
puti.callFunction putTens, puti.parameter(0)
puti.callFunction putOnes, puti.parameter(0)
puti.callFunction newline
puti.return()

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

fibFrom = mod.makeFunction "fibFromTo", vd, i32

from = fibFrom.parameter 0

max = fibFrom.value i32, MAX_FIB
ifBig = fibFrom.if fibFrom.uGreaterThan from, max
ifBig.then.return()

num = fibFrom.callFunction fib, from
fibFrom.callFunction puti, num
next = fibFrom.add from, fibFrom.value(i32, 1)
fibFrom.callFunction fibFrom, next
fibFrom.return()

main.callFunction fibFrom, main.value(i32, 0)
main.return 0

mod.writeBitcodeToFile "fib.bc"
mod.dump()
