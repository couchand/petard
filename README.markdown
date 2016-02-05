petard
======

a js llvm library

        For 'tis the sport to have the enginer
        Hoist with his own petar'...

                    - Hamlet, Shakespeare

  * introduction
  * dependencies
  * getting started
  * documentation
  * more information

[![Build Status](https://travis-ci.org/couchand/petard.svg?branch=master)](https://travis-ci.org/couchand/petard)

introduction
------------

Frustrated with the state of JS support for LLVM, I built another library.

dependencies
------------

  * a recent version of node
  * nan version 2.1
  * llvm version 3.6

getting started
---------------

First you need to install LLVM version 3.6 on your system, including the dev libs.
Look to the Travis build config for details.

when you're building **petard** you will need to set the environment variable
`LLVM_CONFIG` to be the llvm-config you've installed.  For example, if you're on
Ubuntu and it's `llvm-config-3.6`, you'll type `LLVM_CONFIG=llvm-config-3.6` on
the command line before the `npm i` or `npm run build`.

Once you've got it built successfully, try this:

```coffeescript
llvm = require 'petard'

hello = llvm.createModule "hello"

main = hello.createFunction "main", llvm.type.i32
puts = hello.declareFunction "puts", llvm.type.i32, llvm.type.pointerTo llvm.type.i8

text = hello.constant "Hello, world!\n"

message = main.loadConstant text
main.callFunction puts, message

main.return 0

hello.writeBitcodeToFile "hello.bc"
```

Then use opt and clang as usual with the LLVM IR bitcode file.

documentation
-------------

The underlying C++ API exposes the basics of LLVM IR construction, and the node
wrapper adds some higher-level functionality on top of that.  In the listing the
methods in JavaScript are marked with a *(js)*.

  * petard types
    * Type
    * Value
    * Builder
    * FunctionBuilder
    * CodeUnit
  * helpers
    * `type` dict

### petard types

#### Type

Represents an LLVM type.  You'll see these most commonly when you get them from
`petard.type` and pass them when constructing functions and values.  The only
thing you can do to a type is get its name as a string.

##### String type.toString()

Returns the type as a string.

#### Value

Represents an LLVM value, or if you look at it another way, represents an
abstract expression.  You'll get these and pass them in to many of the builder
methods.  The only thing you can do to a value is get its type.

##### Type value.type

A property containing the type of the value.

#### Builder

Something that can construct LLVM IR.  It represents a particular part of a
function (an LLVM block) as well as a cursor within that block where
instructions will be placed.  Calls to builder methods construct LLVM IR and
take and return values and other builders.

##### return(\[Number|Value value\])

Return from the function.  If a value is provided, it is the return value of the
function.  You must ensure that the value passed is of the same type as the
return type of the function.

##### Value parameter(Number index)

Reference function parameter `index`.

##### Value loadConstant(Value constant)

Load a constant value.  Currently the only usage is to load a string constant,
see the first example.

##### Value callFunction(FunctionBuilder|FunctionValue fn, \[Value params...\])

Call the given internal or external function, passing in the parameter values.

##### Value alloca(Type type, \[Number|Value arraySize\])

Allocate space the size of type on the stack.  If an arraySize is provided,
instead allocate that much space times the arraySize.  Returns a pointer value
to the space.

##### Value load(Value pointer)

Load a value from the memory pointed to by pointer.

##### store(Value value, Value pointer)

Store the value in the memory pointed to by pointer.

##### Value add(Value left, Value right)

Add the values.

##### Value sub(Value left, Value right)

Subtract the right value from the left.

##### Value mul(Value left, Value right)

Multiply the values.

##### Value udiv(Value left, Value right)

Unsigned integer divide the left value by the right.

##### Value sdiv(Value left, Value right)

Signed integer divide the left value by the right.

##### Value urem(Value left, Value right)

The remainder when the left value is unsigned integer divided by the right.

##### Value srem(Value left, Value right)

The remainder when the left value is signed integer divided by the right.

##### Value and(Value left, Value right)

Bitwise and the values.

##### Value or(Value left, Value right)

Bitwise or the values.

##### Value xor(Value left, Value right)

Bitwise xor the values.

##### Value shl(Value left, Value right)

Shift the left value left by the right value number of bits.

##### Value lshr(Value left, Value right)

Shift the left value right logically (sign-ignoring) by the right value number
of bits.

##### Value ashr(Value left, Value right)

Shift the left value right arithmetically (sign-preserving) by the right value
number of bits.

##### Value equal(Value left, Value right)

Compare the values for equality.

##### Value notEqual(Value left, Value right)

Compare the values for inequality.

##### Value uGreaterThan(Value left, Value right)

Unsigned greater than comparison.

##### Value uAtLeast(Value left, Value right)

Unsigned greater than or equal to comparison.

##### Value uLessThan(Value left, Value right)

Unsigned less than comparison.

##### Value uAtMost(Value left, Value right)

Unsigned less than or equal to comparison.

##### Value sGreaterThan(Value left, Value right)

Signed greater than comparison.

##### Value sAtLeast(Value left, Value right)

Signed greater than or equal to comparison.

##### Value sLessThan(Value left, Value right)

Signed less than comparison.

##### Value sAtMost(Value left, Value right)

Signed less than or equal to comparison.

##### Value select(Value condition, Value ifTrue, Value ifFalse)

Non-branching value select.  Returns the value corresponding to the ifTrue value
if the condition is true, and the ifFalse value if the condition is not true.

##### Value value(Type type, Any value)

Construct a new LLVM value of the given type for the given JavaScript value.

##### br(Builder target)

Unconditional branch to the target.

##### br(Value condition, Builder ifTrue, Builder ifFalse)

Conditional branch.  If the condition is true, branch to the ifTrue builder,
otherwise branch to the ifFalse builder.

##### Builder createBlock(String name)

Create another block in the same function.  Use with the branching instructions
to create custom control structures.  See the node wrapper for examples on
using `createBlock`, `splitBlock`, `useBlock`, `insertBefore`, `insertAfter`,
and `br` to build control structures.

##### Builder splitBlock(String name)

Split the current block at the cursor.  The object itself retains the top half
(all the instructions written _before_) and the returned builder takes the
bottom half (all the instructions written _after_).  In practice this usually
means the terminator of the block is the only instruction in the new block.

##### useBlock(Builder target)

Set this builder to start building in the target block.  Make sure this builder
already has a terminator or you'll get into trouble.

##### insertAfter()

Start inserting instructions _after_ the cursor rather than before.  Mainly
useful to appending a terminator to a block ahead of time.

##### insertBefore()

Resume inserting instructions _before_ the cursor, the default behavior.  Used
to restore the standard mode after inserting a terminator.

#### FunctionBuilder

A specialization of a builder corresponding to a complete function.  In addition
to the normal builder methods, it has a few special properties.

##### String fnBuilder.name

A property containing the name of the function.

##### Type fnBuilder.type

A property containing the type of the function.

#### CodeUnit

Represents an overall unit of code, such as a file.  Directly corresponds to an
LLVM Module.  It has a few helpful methods.

##### Value unit.constant(Value value)

Compile a constant value into the IR.  At the moment the only use is to load a
constant string.  See the first example for usage.

##### Value unit.declareFunction(String name, \[Type returns\], \[Type takes...\])

Declare a function external to this code unit that will be linked in.  Takes the
name of the function and optionally the return type and parameter types.

##### FunctionBuilder unit.makeFunction(String name, \[Type returns\], \[Type takes...\])

Create a new function in this code unit with the given name and optionall the
return type and parameter types.

##### void unit.dump()

Dump the code unit IR to the screen.  Useful for debugging, not so much for
regular usage, since the underlying LLVM method always dumps to stderr.

##### void unit.writeBitcodeToFile(String filename)

Write the bitcode for this code unit to a file with the given filename.  The
file must not exist, if it does this method will fail.

### helpers

#### `type` dict

This dict contains all the basic types for easy reference.

  * `i1` A one bit integer (a boolean).
  * `i8` A single byte.
  * `i16` A small word.
  * `i32` A medium word.
  * `i64` A long word.
  * `Type pointerTo(Type pointee)` Takes a type and returns a pointer to that type.

more information
----------------

  * [LLVM Language Reference][0]
  * [LLVM Programmer's Manual][1]

[0]: http://llvm.org/docs/LangRef.html
[1]: http://llvm.org/docs/ProgrammersManual.html

##### ╭╮☲☲☲╭╮ #####
