# mandelbrot set

llvm = require '../'

{i32, f32} = llvm.type
vd = llvm.type.void

module = llvm.CodeUnit "mandelbrot"

putchar = module.declareFunction "putchar", i32, i32

putn = module.makeFunction "putn", vd, f32

putn.callFunction putchar, putn.fpToSI putn.parameter(0), i32
putn.return()

converger = (->

  c = module.makeFunction "converger", f32, f32, f32, f32, f32, f32

  one = c.value f32, 1
  two = c.value f32, 2

  r = c.parameter 0
  i = c.parameter 1
  it = c.parameter 2
  cr = c.parameter 3
  ci = c.parameter 4

  rsq = c.mul r, r
  isq = c.mul i, i

  squares = c.add rsq, isq

  max = c.value f32, 255

  itAboveMax = c.foGreaterThan it, max

  squaresAboveFour = c.foGreaterThan squares, c.value f32, 4

  cond = c.or itAboveMax, squaresAboveFour

  ifDone = c.if cond

  e = ifDone.else

  first = e.add(
    e.sub rsq, isq
    cr
  )

  second = e.add(
    e.mul two, e.mul r, i
    ci
  )

  third = e.add it, one

  e.return e.callFunction c, first, second, third, cr, ci

  c.return it

  c

)()

converge = module.makeFunction "converge", f32, f32, f32

_r = converge.parameter 0
_i = converge.parameter 1
zero = converge.value f32, 0

converge.return converge.callFunction converger, _r, _i, zero, _r, _i

printd = (->

  p = module.makeFunction "printd", vd, f32

  d = p.parameter 0

  charIt = (a, b) ->
    limit: p.value f32, a
    char: p.value f32, b

  def = p.value f32, 32

  lower = charIt 4, 42
  low = charIt 8, 46
  high = charIt 16, 43
  higher = charIt 32, 32

  isHigher = p.if p.foGreaterThan d, higher.limit
  isHigher.then.callFunction putn, higher.char

  isHigh = isHigher.else.if p.foGreaterThan d, high.limit
  isHigh.then.callFunction putn, high.char

  isLow = isHigh.else.if p.foGreaterThan d, low.limit
  isLow.then.callFunction putn, low.char

  isLower = isLow.else.if p.foGreaterThan d, lower.limit
  isLower.then.callFunction putn, lower.char

  isLower.else.callFunction putn, def

  p.return()

  p

)()

newline = module.makeFunction "newline"
newline.callFunction putchar, newline.value i32, 10
newline.return()

mandelrow = (->

  r = module.makeFunction "mandelrow", vd, f32, f32, f32, f32

  row = r.parameter 0
  col = r.parameter 1
  max = r.parameter 2
  step = r.parameter 3

  ifSmall = r.if r.foLessThan col, max

  res = ifSmall.then.callFunction converge, col, row
  ifSmall.then.callFunction printd, res

  nextcol = ifSmall.then.add col, step
  ifSmall.then.callFunction r, row, nextcol, max, step

  r.return()

  r

)()

mandelhelp = (->

  h = module.makeFunction "mandelhelp", vd, f32, f32, f32, f32, f32, f32

  row = h.parameter 0
  rowmax = h.parameter 1
  rowstep = h.parameter 2
  colmin = h.parameter 3
  colmax = h.parameter 4
  colstep = h.parameter 5

  ifSmall = h.if h.foLessThan row, rowmax

  ifSmall.then.callFunction mandelrow, row, colmin, colmax, colstep
  ifSmall.then.callFunction newline

  nextrow = ifSmall.then.add row, rowstep
  ifSmall.then.callFunction h, nextrow, rowmax, rowstep, colmin, colmax, colstep

  h.return()

  h

)()

mandel = (->

  m = module.makeFunction "mandel", vd, f32, f32, f32, f32, f32, f32

  xmin = m.parameter 0
  xmax = m.parameter 1
  width = m.parameter 2
  ymin = m.parameter 3
  ymax = m.parameter 4
  height = m.parameter 5

  xstep = m.fdiv m.sub(xmax, xmin), width
  ystep = m.fdiv m.sub(ymax, ymin), height

  m.callFunction mandelhelp, ymin, ymax, ystep, xmin, xmax, xstep

  m.return()

  m

)()

main = module.makeFunction "main"

negativeTwo = main.value f32, -2
half = main.value f32, 0.5
seventyEight = main.value f32, 78
negativeOne = main.value f32, -1
one = main.value f32, 1
thirtySix = main.value f32, 36

main.callFunction mandel, negativeTwo, one, seventyEight, negativeOne, one, thirtySix

main.return()

module.dump()
module.writeBitcodeToFile "mandelbrot.bc"
