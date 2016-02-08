# language example cli

{
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
} = require './ast'

{
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
} = require './parser'

compile = (name, code) ->
  unit = parseCodeUnit name, code
  unit.typecheck()
  unit.compile()

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
