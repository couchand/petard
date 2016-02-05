# language example
#
# example of usage:
#
# def fib(i) {
#   if (i == 0) {
#     ret 1;
#   }
#   if (i == 1) {
#     ret 1;
#   }
#   ret fib(i - 1) + fib(i - 2);
# }
#
# def main() {
#   ret fib(10);
# }

# ast nodes

llvm = require '../'

class IntLiteral
  constructor: (@value) ->
  compile: (builder, fns, params, vars) ->
    builder.value llvm.type.i32, @value

class Variable
  constructor: (@name) ->
  compile: (builder, fns, params, vars) ->
    if @name of vars
      builder.load vars[@name]
    else if @name of params
      params[@name]

class FunctionCall
  constructor: (@fn, @args) ->
  compile: (builder, fns, params, vars) ->
    args = for arg in @args
      arg.compile builder, fns, params, vars

    fn = fns[@fn]

    args.unshift fn

    builder.callFunction.apply builder, args

class BinaryExpression
  constructor: (@left, @op, @right) ->
  compile: (builder, fns, params, vars) ->
    l = @left.compile builder, fns, params, vars
    r = @right.compile builder, fns, params, vars
    switch @op
      when '==' then builder.equal l, r
      when '+' then builder.add l, r
      when '-' then builder.sub l, r
      when '<' then builder.sLessThan l, r

class ReturnStatement
  constructor: (@expr) ->
  compile: (builder, fns, params, vars) ->
    builder.return @expr.compile builder, fns, params, vars

class AssignmentStatement
  constructor: (@name, @expr) ->
  compile: (builder, fns, params, vars) ->
    location = if @name of vars
      vars[@name]
    else
      builder.alloca llvm.type.i32

    val = @expr.compile builder, fns, params, vars
    builder.store val, location

    unless @name of vars
      vars[@name] = location

class IfStatement
  constructor: (@condition, @then, @else) ->
  compile: (builder, fns, params, vars) ->
    ifTrue = builder.if @condition.compile builder, fns, params, vars

    for statement in @then
      statement.compile ifTrue.then, fns, params, vars

    if @else
      for statement in @else
        statement.compile ifTrue.else, fns, params, vars

class WhileStatement
  constructor: (@condition, @body) ->
  compile: (builder, fns, params, vars) ->
    c = @condition
    bod = builder.while (w) ->
      c.compile w, fns, params, vars

    for statement in @body
      statement.compile bod, fns, params, vars

class FunctionDefinition
  constructor: (@name, @parameters, @body) ->
  compile: (builder, fns) ->
    params = {}
    for i, param of @parameters
      params[param] = builder.parameter +i

    vars = {}
    for statement in @body
      statement.compile builder, fns, params, vars

class CodeUnit
  constructor: (@name, @fns) ->
  compile: ->
    unit = new llvm.CodeUnit @name

    fns = {}

    for fn in @fns
      params = [fn.name, llvm.type.i32]
      for _ in fn.parameters
        params.push llvm.type.i32

      b = fns[fn.name] = unit.makeFunction.apply unit, params

      fn.compile b, fns

    unit

# parsing

parseIntLiteral = (str) ->
  if /^[0-9]/.test str
    match = /^[0-9]+/.exec str
    val = parseInt match[0]
    return [str[match[0].length..], new IntLiteral val]

parseVariable = (str) ->
  if /^[_a-zA-Z]/.test str
    match = /^[_a-zA-Z][_a-zA-Z0-9]*/.exec str
    name = match[0]
    return [str[match[0].length..], new Variable name]

parseFunctionCall = (str) ->
  if /^[_a-zA-Z][_a-zA-Z0-9]*\s*\(\s*/.test str
    match = /^([_a-zA-Z][_a-zA-Z0-9]*)\s*\(\s*/.exec str
    name = match[1]
    rest = str[match[0].length..]
    
    params = []

    child = parseExpression rest
    if child
      params.push child[1]
      rest = child[0]

    while /^\s*,/.test rest
      match2 = /^\s*,\s*/.test rest

      rest2 = rest[match2[0].length..]

      child2 = parseExpression rest2
      params.push child2[1]
      rest = child2[0]

    closing = /^\s*\)\s*/.exec rest

    return unless closing
    
    [rest[closing[0].length..], new FunctionCall(name, params)]

# TODO: fix precedence
parseBinaryExpression = (str) ->
  leftP = parsePrimary str
  return unless leftP

  next = leftP[0]
  left = leftP[1]

  return unless /^\s*(==|[<+-])\s*/.test next

  match = /^\s*(==|[<+-])\s*/.exec next
  op = match[1]
  rest = next[match[0].length..]

  rightP = parseExpression rest
  return unless rightP

  last = rightP[0]
  right = rightP[1]

  [last, new BinaryExpression left, op, right]

parseExpression = (str) ->
  binP = parseBinaryExpression str
  return binP if binP

  primP = parsePrimary str
  return primP if primP

parsePrimary = (str) ->
  callP = parseFunctionCall str
  return callP if callP

  varP = parseVariable str
  return varP if varP

  intP = parseIntLiteral str
  return intP if intP

parseReturnStatement = (str) ->
  ret = /^\s*ret\s+/.exec str
  return unless ret
  next = str[ret[0].length..]

  body = parseExpression next

  semi = /^\s*;\s*/.exec body
  return unless semi

  [body[0][semi[0].length..], new ReturnStatement body[1]]

parseAssignmentStatement = (str) ->
  nameP = parseVariable str
  return unless nameP
  next = nameP[0]
  name = nameP[1].name

  assignM = /\s*=\s*/.exec next
  return unless assignM
  next = next[assignM[0].length..]

  exprP = parseExpression next
  return unless exprP

  semi = /^\s*;\s*/.exec exprP[0]
  return unless semi

  rest = exprP[0][semi[0].length..]

  [rest, new AssignmentStatement name, exprP[1]]

parseIfStatement = (str) ->
  ifm = /^\s*if\s*\(\s*/.exec str
  return unless ifm
  next = str[ifm[0].length..]

  condM = parseExpression next
  return unless condM
  next = condM[0]
  cond = condM[1]

  openBrace = /^\s*\)\s*{\s*/.exec next
  return unless openBrace

  thenM = parseBlock next[openBrace[0].length..]
  return unless thenM
  thenC = thenM[1]

  closeBrace = /^\s*}\s*/.exec thenM[0]
  return unless closeBrace

  next2 = thenM[0][closeBrace[0].length..]

  elseC = undefined
  if /^\s*else\s*{/.test next2
    elseM = /^\s*else\s*{\s*/
    next3 = next2[elseM[0].length..]
    elseM = parseBlock next3
    return unless elseM
    elseC = elseM[1]

    closeBrace2 = /^\s*}\s*/.exec elseM[0]
    next2 = next3[closeBrace2[0].length..]

  [next2, new IfStatement cond, thenC, elseC]

parseWhileStatement = (str) ->
  whilem = /^\s*while\s*\(\s*/.exec str
  return unless whilem
  next = str[whilem[0].length..]

  condP = parseExpression next
  return unless condP
  next = condP[0]
  cond = condP[1]

  openBrace = /^\s*\)\s*{\s*/.exec next
  return unless openBrace

  bodyM = parseBlock next[openBrace[0].length..]
  return unless bodyM
  body = bodyM[1]

  closeBrace = /^\s*}\s*/.exec bodyM[0]
  return unless closeBrace

  next2 = bodyM[0][closeBrace[0].length..]

  [next2, new WhileStatement cond, body]

parseStatement = (str) ->
  ifS = parseIfStatement str
  return ifS if ifS

  whileS = parseWhileStatement str
  return whileS if whileS

  retS = parseReturnStatement str
  return retS if retS

  assignS = parseAssignmentStatement str
  return assignS if assignS

parseBlock = (str) ->
  statements = []

  val = str
  while stM = parseStatement val
    statements.push stM[1]
    val = stM[0]

  [val, statements]

parseFunctionDefinition = (str) ->
  defm = /^\s*def\s*/.exec str
  return unless defm
  next = str[defm[0].length..]

  nameM = parseVariable next
  return unless nameM
  next = nameM[0]
  name = nameM[1].name

  openP = /\s*\(\s*/.exec next
  return unless openP
  next = next[openP[0].length..]

  varsM = parseParameters next
  return unless varsM
  next = varsM[0]
  vars = varsM[1]

  openBrace = /^\s*\)\s*{\s*/.exec next
  return unless openBrace

  bodyM = parseBlock next[openBrace[0].length..]
  return unless bodyM
  body = bodyM[1]

  closeBrace = /^\s*}\s*/.exec bodyM[0]
  return unless closeBrace

  next2 = bodyM[0][closeBrace[0].length..]

  [next2, new FunctionDefinition name, vars, body]

parseParameters = (str) ->
  parameters = []

  first = parseVariable str
  return [str, []] unless first

  parameters.push first[1].name
  next = first[0]

  while comma = /\s*,\s*/.exec next
    next = next[comma[0].length..]

    following = parseExpression next
    return [next, parameters] unless following

    parameters.push following[1].name
    next = following[0]

  [next, parameters]

parseCodeUnit = (name, str) ->
  next = str
  fns = []
  while fn = parseFunctionDefinition next
    next = fn[0]
    fns.push fn[1]
  new CodeUnit name, fns

if module isnt require.main
  module.exports = {
    IntLiteral
    Variable
    FunctionCall
    BinaryExpression
    ReturnStatement
    IfStatement
    FunctionDefinition
    CodeUnit
  
    parseIntLiteral
    parseVariable
    parseFunctionCall
    parseBinaryExpression
    parseExpression
    parseReturnStatement
    parseAssignmentStatement
    parseIfStatement
    parseWhileStatement
    parseStatement
    parseBlock
    parseFunctionDefinition
    parseCodeUnit
  }
  return

usage = ->
  console.error "Usage: coffee example/language.coffee <INPUT_FILE>"
  process.exit 1

if process.argv.length <= 2
  usage()

inputFile = process.argv[2]

fs = require 'fs'

input = fs.readFileSync inputFile
  .toString()

unit = parseCodeUnit inputFile, input
mod = unit.compile()
mod.dump()
mod.writeBitcodeToFile inputFile + ".bc"
