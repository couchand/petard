# language example ast and compiler

llvm = require '../../'

getType = (ty) ->
  switch ty
    when 'int' then llvm.type.i32
    when 'float' then llvm.type.f32

class IntLiteral
  constructor: (@value) ->
  typecheck: ->
    llvm.type.i32
  compile: (builder, fns, params, vars) ->
    builder.value llvm.type.i32, @value

class FloatLiteral
  constructor: (@value) ->
  typecheck: ->
    llvm.type.f32
  compile: (builder) ->
    builder.value llvm.type.f32, @value

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
        throw new Error "incompatible types in call to #{@fn} (parameter #{i}): #{pt.toString()} and #{at.toString()}"

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
    @ty = switch @op
      when '==', '<' then llvm.type.i1
      else lt

  compile: (builder, fns, params, vars) ->
    l = @left.compile builder, fns, params, vars
    r = @right.compile builder, fns, params, vars
    switch @op
      when '+' then builder.add l, r
      when '-' then builder.sub l, r
      when '*' then builder.mul l, r
      when '<'
        if @ty.isFloatType
          builder.foLessThan l, r
        else
          builder.sLessThan l, r
      when '=='
        if @ty.isFloatType()
          builder.foEqual l, r
        else
          builder.equal l, r
      when '/'
        if @ty.isFloatType()
          builder.fdiv l, r
        else
          builder.sdiv l, r

class ReturnStatement
  constructor: (@expr) ->
  typecheck: ->
    @expr.typecheck.apply @expr, arguments
  compile: (builder, fns, params, vars) ->
    builder.return @expr.compile builder, fns, params, vars

class DeclarationStatement
  constructor: (@type, @name, @expr) ->
  typecheck: (fntys, paramtys, vartys) ->
    if @name of vartys or @name of paramtys
      throw new Error "redeclaration of #{@name}"

    exprty = @expr.typecheck fntys, paramtys, vartys

    varty = getType @type
    unless varty
      throw new Error "unknown type in declaration: #{@type}"

    unless exprty.isCompatibleWith varty
      throw new Error "incompatible types in declaration: #{varty.toString()} and #{exprty.toString()}"

    vartys[@name] = varty

  compile: (builder, fns, params, vars) ->
    varty = getType @type
    unless varty
      throw new Error "unknown type in declaration: #{@type}"

    location = builder.alloca varty
    val = @expr.compile builder, fns, params, vars

    builder.store val, location

    vars[@name] = location

class AssignmentStatement
  constructor: (@name, @expr) ->
  typecheck: (fntys, paramtys, vartys) ->
    locationty = if @name of vartys
      vartys[@name]
    else
      throw new Error "unknown value in assignment: #{@name}"

    exprty = @expr.typecheck fntys, paramtys, vartys

    unless locationty.isCompatibleWith exprty
      throw new Error "incompatible types in assignment: #{locationty.toString()} and #{exprty.toString()}"

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

class FunctionDeclaration
  constructor: (@name, @returns, @parameters) ->
  typecheck: (fntys) ->
    ptys = []
    for param in @parameters
      p = getType param[0]
      unless p
        throw new Error "unknown type in function declaration: #{param[0]}"
      ptys.push p

    returnty = getType @returns
    unless returnty
      throw new Error "unknown type in function declaration: #{@returns}"
    ptys.unshift returnty

    @ty = fntys[@name] = llvm.getFunctionTy.apply llvm, ptys

    console.log 'fn type', @ty.toString()

    @ty

  compile: (unit, fns) ->
    init = [@name, @ty.returns].concat @ty.parameters
    fns[@name] = unit.declareFunction.apply unit, init

class FunctionDefinition
  constructor: (@name, @returns, @parameters, @body) ->
  typecheck: (fntys) ->
    paramtys = {}
    ptys = []
    for i, param of @parameters
      p = getType param[0]
      unless p
        throw new Error "unknown type in function definition: #{param[0]}"
      ptys.push paramtys[param[1]] = p

    ptys.unshift returnty = getType @returns
    unless returnty
      throw new Error "unknown type in function definition: #{@returns}"

    @ty = fntys[@name] = llvm.getFunctionTy.apply llvm, ptys

    vartys = {}
    for statement in @body
      ty = statement.typecheck fntys, paramtys, vartys

      if statement instanceof ReturnStatement
        unless returnty.isCompatibleWith ty
          throw new Error "incompatible type in return: #{returnty.toString()} and #{ty.toString()}"

  compile: (unit, fns) ->
    init = [@name, @ty.returns].concat @ty.parameters
    builder = unit.makeFunction.apply unit, init
    fns[@name] = builder

    params = {}
    for i, param of @parameters
      params[param[1]] = builder.parameter +i

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
      fn.compile unit, fns

    unit

module.exports = {
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
  FunctionDeclaration
  FunctionDefinition
  CodeUnit
}
