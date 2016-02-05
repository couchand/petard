# fib-iterative

llvm = require '../'

i32 = llvm.type.i32

mod = new llvm.CodeUnit "fib-iterative"

f = mod.makeFunction "f", i32, i32

nlesstwofib = f.alloca i32
nlessonefib = f.alloca i32
n = f.alloca i32

target = f.parameter 0

one = f.value i32, 1
f.store one, nlesstwofib
f.store one, nlessonefib

zero = f.value i32, 0
f.store zero, n

body = f.while (w) ->
  w.uLessThan w.load(n), target

nval = body.load n

nlesstwoval = body.load nlesstwofib
nlessoneval = body.load nlessonefib

body.store nlessoneval, nlesstwofib
body.store body.add(nlessoneval, nlesstwoval), nlessonefib

next = body.add nval, body.value(i32, 1)
body.store next, n

f.return f.load nlesstwofib

main = mod.makeFunction "main", i32

main.return main.callFunction f, main.value i32, 10

mod.dump()
mod.writeBitcodeToFile 'fib-iterative.bc'
