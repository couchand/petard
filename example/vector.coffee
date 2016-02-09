# vector example

llvm = require '../'

{i8, i32, f32, vectorOf, structOf, pointerTo} = llvm.type

vec3ty = vectorOf 3, f32

mod = llvm.CodeUnit 'vector'

makeVector = mod.makeFunction 'makeVector', vec3ty, f32, f32, f32
withX = makeVector.insertElement makeVector.undefined(vec3ty), makeVector.parameter(0), 0
withY = makeVector.insertElement withX, makeVector.parameter(1), 1
withZ = makeVector.insertElement withY, makeVector.parameter(2), 2
makeVector.return withZ

getX = mod.makeFunction 'getX', f32, vec3ty
getX.return getX.extractElement getX.parameter(0), 0

getY = mod.makeFunction 'getY', f32, vec3ty
getY.return getY.extractElement getY.parameter(0), 1

getZ = mod.makeFunction 'getZ', f32, vec3ty
getZ.return getZ.extractElement getZ.parameter(0), 2

dot = mod.makeFunction 'dot', f32, vec3ty, vec3ty
products = dot.mul dot.parameter(0), dot.parameter(1)
x = dot.callFunction getX, products
y = dot.callFunction getY, products
z = dot.callFunction getZ, products
dot.return dot.add x, dot.add y, z

broadcast = mod.makeFunction 'broadcast', vec3ty, f32
p = broadcast.parameter 0
broadcast.return broadcast.callFunction makeVector, p, p, p

mod.dump()
