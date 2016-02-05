# switch statements

# expected output:

# one
# two
# three
# four
# five
# six

llvm = require '../'

{i1, i8, i32, pointerTo} = llvm.type
vd = llvm.type.void

sw = llvm.CodeUnit "switch"

puts = sw.declareFunction "puts", i32, pointerTo i8

main = sw.makeFunction "main", i32
countTo = sw.makeFunction "countTo", vd, i32, i32
sayNum = sw.makeFunction "sayNum", vd, i32

# sayNum

oneP = sw.constant "one"
twoP = sw.constant "two"
threeP = sw.constant "three"
fourP = sw.constant "four"
fiveP = sw.constant "five"
sixP = sw.constant "six"

oneS = sayNum.loadConstant oneP
twoS = sayNum.loadConstant twoP
threeS = sayNum.loadConstant threeP
fourS = sayNum.loadConstant fourP
fiveS = sayNum.loadConstant fiveP
sixS = sayNum.loadConstant sixP

sayIt = sayNum.select sayNum.parameter(0)
oneB = sayIt.addCase 1
twoB = sayIt.addCase 2
threeB = sayIt.addCase 3
fourB = sayIt.addCase 4
fiveB = sayIt.addCase 5
sixB = sayIt.addCase 6

oneB.callFunction puts, oneS
twoB.callFunction puts, twoS
threeB.callFunction puts, threeS
fourB.callFunction puts, fourS
fiveB.callFunction puts, fiveS
sixB.callFunction puts, sixS

sayNum.return()

# countTo

countTo.callFunction sayNum, countTo.parameter(0)

ifNotDone = countTo.if countTo.uLessThan countTo.parameter(0), countTo.parameter(1)
more = ifNotDone.then

more.callFunction countTo, more.add(countTo.parameter(0), more.value(i32, 1)), countTo.parameter(1)

countTo.return()

# main

oneV = main.value i32, 1
sixV = main.value i32, 6

main.callFunction countTo, oneV, sixV

main.return 0

sw.writeBitcodeToFile "switch.bc"
sw.dump()
