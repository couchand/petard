# language example ast and compiler

llvm = require '../../'

class IntLiteral
  constructor: (@value) ->
  typecheck: ->
    llvm.type.i32
  compile: (builder, fns, params, vars) ->
    builder.value llvm.type.i32, @value

class Variable
  constructor: (@name) ->
  typecheck: (fntys, paramtys, vartys) ->
    if @name of vartys
      vartys[@name]
    else
      paramtys[@name]
  compile: (builder, fns, params, vars) ->
    if @name of vars
      builder.load vars[@name]
    else if @name of params
      params[@name]

class FunctionCall
  constructor: (@fn, @args) ->
  typecheck: (fntys, paramtys, vartys) ->
    argtys = for arg in @args
      arg.typecheck fntys, paramtys, vartys

    fnty = fntys[@fn]

    for i in [0...fnty.parameters.length]
      at = argtys[i]
      pt = fnty.parameters[i]

      unless pt.isCompatibleWith at
        throw new Error "incompatible types in call to #{@fn}[#{i}]: #{pt.toString()} and #{at.tostring()}"

    fnty.returns
  compile: (builder, fns, params, vars) ->
    args = for arg in @args
      arg.compile builder, fns, params, vars

    fn = fns[@fn]

    args.unshift fn

    builder.callFunction.apply builder, args

class BinaryExpression
  constructor: (@left, @op, @right) ->
  typecheck: (fntys, paramtys, vartys) ->
    lt = @left.typecheck fntys, paramtys, vartys
    rt = @right.typecheck fntys, paramtys, vartys
    unless lt.isCompatibleWith rt
      throw new Error "incompatible types in binary #{@op}: #{lt.toString()} and #{rt.toString()}"
    switch @op
      when '==', '<' then return llvm.type.i1
      else return lt
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
  typecheck: ->
    @expr.typecheck.apply @expr, arguments
  compile: (builder, fns, params, vars) ->
    builder.return @expr.compile builder, fns, params, vars

class AssignmentStatement
  constructor: (@name, @expr) ->
  typecheck: (fntys, paramtys, vartys) ->
    locationty = if @name of vartys
      vartys[@name]
    else
      llvm.type.i32

    exprty = @expr.typecheck fntys, paramtys, vartys

    unless locationty.isCompatibleWith exprty
      throw new Error "incompatible types in assignment: #{locationty.toString()} and #{exprty.toString()}"

    unless @name of vartys
      vartys[@name] = locationty

    locationty

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
  typecheck: (fntys, paramtys, vartys) ->
    condty = @condition.typecheck fntys, paramtys, vartys
    unless condty.isCompatibleWith llvm.type.i1
      throw new Error "condition must be a boolean"

    for statement in @then
      statement.typecheck fntys, paramtys, vartys

    if @else
      for statement in @else
        statement.typecheck fntys, paramtys, vartys

  compile: (builder, fns, params, vars) ->
    ifTrue = builder.if @condition.compile builder, fns, params, vars

    for statement in @then
      statement.compile ifTrue.then, fns, params, vars

    if @else
      for statement in @else
        statement.compile ifTrue.else, fns, params, vars

class WhileStatement
  constructor: (@condition, @body) ->
  typecheck: (fntys, paramtys, vartys) ->
    condty = @condition.typecheck fntys, paramtys, vartys
    unless condty.isCompatibleWith llvm.type.i1
      throw new Error "condition must be a boolean"
    for statement in @body
      statement.typecheck fntys, paramtys, vartys

  compile: (builder, fns, params, vars) ->
    c = @condition
    bod = builder.while (w) ->
      c.compile w, fns, params, vars

    for statement in @body
      statement.compile bod, fns, params, vars

class FunctionDefinition
  constructor: (@name, @parameters, @body) ->
  typecheck: (fntys) ->
    paramtys = {}
    ptys = []
    for i, param of @parameters
      ptys.push paramtys[param] = llvm.type.i32

    ptys.push returnty = llvm.type.i32

    fntys[@name] = llvm.getFunctionTy.apply llvm, ptys

    vartys = {}
    for statement in @body
      statement.typecheck fntys, paramtys, vartys

  compile: (builder, fns) ->
    params = {}
    for i, param of @parameters
      params[param] = builder.parameter +i

    vars = {}
    for statement in @body
      statement.compile builder, fns, params, vars

class CodeUnit
  constructor: (@name, @fns) ->
  typecheck: ->
    fntys = {}

    for fn in @fns
      fn.typecheck fntys

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

module.exports = {
  IntLiteral
  Variable
  FunctionCall
  BinaryExpression
  ReturnStatement
  AssignmentStatement
  IfStatement
  WhileStatement
  FunctionDefinition
  CodeUnit
}
