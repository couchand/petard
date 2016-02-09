# language example parser

{
  IntLiteral
  FloatLiteral
  Variable
  FunctionCall
  BinaryExpression
  ReturnStatement
  DeclarationStatement
  AssignmentStatement
  IfStatement
  WhileStatement
  FunctionDefinition
  CodeUnit
} = require './ast'

parseIntLiteral = (str) ->
  if /^[0-9]/.test str
    match = /^[0-9]+/.exec str
    val = parseInt match[0]
    return [str[match[0].length..], new IntLiteral val]

parseFloatLiteral = (str) ->
  wholeP = parseIntLiteral str
  return unless wholeP
  next = wholeP[0]
  whole = wholeP[1]

  match = /^\.[0-9]+/.exec next
  return unless match

  [next[match[0].length..], new FloatLiteral parseFloat "#{whole.value}#{match[0]}"]

parseVariable = (str) ->
  if /^[_a-zA-Z]/.test str
    match = /^([_a-zA-Z][_a-zA-Z0-9]*)\s*/.exec str
    name = match[1]
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

  floatP = parseFloatLiteral str
  return floatP if floatP

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

parseDeclarationStatement = (str) ->
  typeP = parseVariable str
  return unless typeP
  next = typeP[0]
  type = typeP[1].name

  nameP = parseVariable next
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

  [rest, new DeclarationStatement type, name, exprP[1]]

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

  declareS = parseDeclarationStatement str
  return declareS if declareS

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

  typeM = parseVariable next
  return unless typeM
  next = typeM[0]
  type = typeM[1].name

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

  [next2, new FunctionDefinition name, type, vars, body]

parseParameters = (str) ->
  parameters = []

  firstTy = parseVariable str
  return [str, []] unless firstTy

  firstName = parseVariable firstTy[0]
  return [str, []] unless firstName

  parameters.push [firstTy[1].name, firstName[1].name]
  next = firstName[0]

  while comma = /\s*,\s*/.exec next
    next = next[comma[0].length..]

    followingTy = parseVariable next
    return [next, parameters] unless followingTy

    followingName = parseVariable followingTy[0]

    parameters.push [followingTy[1].name, followingName[1].name]
    next = followingName[0]

  [next, parameters]

parseCodeUnit = (name, str) ->
  next = str
  fns = []
  while fn = parseFunctionDefinition next
    next = fn[0]
    fns.push fn[1]
  new CodeUnit name, fns

module.exports = {
  parseIntLiteral
  parseFloatLiteral
  parseVariable
  parseFunctionCall
  parseBinaryExpression
  parseExpression
  parseReturnStatement
  parseDeclarationStatement
  parseAssignmentStatement
  parseIfStatement
  parseWhileStatement
  parseStatement
  parseBlock
  parseFunctionDefinition
  parseCodeUnit
}
