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

##### Value unit.declareFunction(String name, Type returns, Type takes...)

Declare a function external to this code unit that will be linked in.  Takes the
name of the function and optionally the return type and parameter types.

##### FunctionBuilder unit.makeFunction(String name, Type returns, Type takes...)

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
