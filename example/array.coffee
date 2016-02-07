# array example

llvm = require '../'

{i32, arrayOf, pointerTo} = llvm.type

mod = llvm.CodeUnit "memory"

arrty = arrayOf 3, i32

addEls = mod.makeFunction 'addEls', i32, pointerTo arrty

firstP = addEls.getElementPointer addEls.parameter(0), 0, 0
secondP = addEls.getElementPointer addEls.parameter(0), 0, 1
thirdP = addEls.getElementPointer addEls.parameter(0), 0, 2

first = addEls.load firstP
second = addEls.load secondP
third = addEls.load thirdP

mid = addEls.add first, second
addEls.return addEls.add mid, third

main = mod.makeFunction 'main', i32

arrP = main.alloca arrty

aP = main.getElementPointer arrP, 0, 0
bP = main.getElementPointer arrP, 0, 1
cP = main.getElementPointer arrP, 0, 2

main.store 3, aP
main.store 5, bP
main.store 7, cP

main.return main.callFunction addEls, arrP

mod.dump()
mod.writeBitcodeToFile 'array.bc'
