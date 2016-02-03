# conditionals

llvm = require '../'

{i1, i8, i32, pointerTo} = llvm.type
vd = llvm.type.void

cond = llvm.CodeUnit "conditional"

puts = cond.declareFunction "puts", i32, pointerTo i8

main = cond.makeFunction "main", i32
greet = cond.makeFunction "greet", vd, i1

helloP = cond.constant "Hello!\n"
goodbyeP = cond.constant "Goodbye!\n"

hello = greet.loadConstant helloP
goodbye = greet.loadConstant goodbyeP

hi = main.value i1, 1
bye = main.value i1, 0

ifHi = greet.if greet.equal hi, greet.parameter 0
ifHi.then.callFunction puts, hello
ifHi.else.callFunction puts, goodbye

greet.return()

main.callFunction greet, hi
main.callFunction greet, bye

main.return 0

cond.writeBitcodeToFile "conditional.bc"
cond.dump()
