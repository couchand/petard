# language example ast and compiler

llvm = require '../../'

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
