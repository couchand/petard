# language example cli

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
  FunctionDeclaration
  CodeUnit
} = require './ast'

{
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
  parseFunctionDeclaration
  parseCodeUnit
} = require './parser'

compile = (name, code) ->
  unit = parseCodeUnit name, code
  console.log unit
  unit.typecheck()
  unit.compile()

if module isnt require.main
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
    FunctionDefinition
    FunctionDeclaration
    CodeUnit

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
    parseFunctionDeclaration
    parseCodeUnit

    compile
  }
  return

usage = ->
  console.error "Usage: coffee index.coffee <INPUT_FILE>"
  process.exit 1

if process.argv.length <= 2
  usage()

inputFile = process.argv[2]

fs = require 'fs'

input = fs.readFileSync inputFile
  .toString()

mod = compile inputFile, input
mod.dump()
mod.writeBitcodeToFile inputFile + ".bc"
