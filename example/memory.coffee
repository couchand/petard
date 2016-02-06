# dynamic memory example

llvm = require '../'

{i8, i32, pointerTo} = llvm.type

hello = llvm.CodeUnit "memory"

getchar = hello.declareFunction "getchar", i32
puts = hello.declareFunction "puts", i32, pointerTo i8

prefixC = hello.constant "Hello, "
suffixC = hello.constant "!\n"

main = hello.makeFunction "main", i32

crlf = main.value i8, 10

zero = main.value i32, 0
one = main.value i32, 1

sizeP = main.alloca i32
main.store 0, sizeP

charP = main.alloca i8
main.store 0, charP

MAX_LENGTH = 10
MAX_LENGTH_VALUE = main.value i32, MAX_LENGTH
nameP = main.alloca i8, main.value i32, MAX_LENGTH + 1

b = main.while (c) ->
  char = c.load charP
  size = c.load sizeP
  c.and(c.notEqual(MAX_LENGTH_VALUE, size),
    c.notEqual crlf, char)

nextch = b.trunc b.callFunction(getchar), i8
b.store nextch, charP

size = b.load sizeP

nextspot = b.getElementPointer nameP, size
b.store nextch, nextspot

nextsz = b.add size, one
b.store nextsz, sizeP

terminatorP = main.getElementPointer nameP, main.load sizeP
main.store 0, terminatorP

prefix = main.loadConstant prefixC
suffix = main.loadConstant suffixC

main.callFunction puts, prefix
main.callFunction puts, nameP
main.callFunction puts, suffix

main.return 0

hello.writeBitcodeToFile "memory.bc"
hello.dump()
