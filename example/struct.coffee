# struct example

llvm = require '..'

{i8, i32, pointerTo, structOf} = llvm.type
vd = llvm.type.void

i8p = pointerTo i8
i32p = pointerTo i32

mod = llvm.CodeUnit 'struct'

objectTy = structOf [i8, i8p]
objectTyP = pointerTo objectTy

# malloc

malloc = (->
  m = mod.makeFunction 'mymalloc', objectTyP, objectTyP, i32p

  topP = m.parameter 1
  top = m.load topP
  m.store m.add(top, m.value i32, 1), topP

  m.return m.getElementPointer m.parameter(0), top

  m
)()

# wrap

wrap = (->
  c = mod.makeFunction 'mywrap', objectTyP, objectTyP, i32p, objectTyP

  newobj = c.callFunction malloc, c.parameter(0), c.parameter(1)

  tagP = c.getElementPointer newobj, c.value(i32, 0), c.value(i32, 0)

  c.store c.value(i8, 1), tagP

  ptrP = c.getElementPointer newobj, c.value(i32, 0), c.value(i32, 1)

  c.store c.bitcast(c.parameter(2), i8p), ptrP

  c.return newobj

  c
)()

# nil

nil = (->
  n = mod.makeFunction 'mynil', objectTyP, objectTyP, i32p

  newobj = n.callFunction malloc, n.parameter(0), n.parameter(1)

  tagP = n.getElementPointer newobj, n.value(i32, 0), n.value(i32, 0)

  n.store n.value(i8, 0), tagP

  n.return newobj

  n
)()

# len

len = (->
  l = mod.makeFunction 'mylen', i32, objectTyP

  tagP = l.getElementPointer l.parameter(0), l.value(i32, 0), l.value(i32, 0)
  tag = l.load tagP

  done = l.if l.equal tag, l.value i8, 0

  done.then.return 0

  ptrP = l.getElementPointer l.parameter(0), l.value(i32, 0), l.value(i32, 1)
  ptr = l.load ptrP

  child = l.bitcast ptr, objectTyP

  length = l.callFunction l, child

  l.return l.add l.value(i32, 1), length

  l
)()

mod.dump()

# test

test = (->
  t = mod.makeFunction 'mytest', i32, objectTyP, i32p

  heap = t.parameter 0
  top = t.parameter 1

  end = t.callFunction nil, heap, top
  next = t.callFunction wrap, heap, top, end
  next2 = t.callFunction wrap, heap, top, next
  next3 = t.callFunction wrap, heap, top, next2
  front = t.callFunction wrap, heap, top, next3

  t.return t.callFunction len, front

  t
)()

# main

main = mod.makeFunction 'main', i32

heapP = main.alloca objectTy, 10

topP = main.alloca i32
main.store 0, topP

main.return main.callFunction test, heapP, topP

mod.dump()
mod.writeBitcodeToFile 'struct.bc'
