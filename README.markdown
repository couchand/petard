petard
======

a js llvm library

        For 'tis the sport to have the enginer
        Hoist with his own petar'...

                    - Hamlet, Shakespeare

  * [introduction][90]
  * [dependencies][91]
  * [getting started][92]
  * [documentation][93]
    * [petard types][15]
      * [Type][10]
      * [Value][11]
      * [Builder][12]
      * [FunctionBuilder][13]
      * [SwitchBuilder][18]
      * [ChooseBuilder][19]
      * [CodeUnit][14]
    * [helpers][16]
      * [`type` dict][17]
  * [more information][94]

[![Build Status](https://travis-ci.org/couchand/petard.svg?branch=master)](https://travis-ci.org/couchand/petard)

introduction
------------

Frustrated with the state of JS support for LLVM, I built another library.
Rather than trying to be generic bindings to libLLVM, **petard** tries to make
the most common case easy: generating fresh IR.  You don't have total control
over the LLVM machinery and there aren't really facilities for working with IR
that already exists (so you can't write passes), but if those limitations aren't
a problem **petard** makes dealing with LLVM much simpler.

This software is under active development.  The README should reflect current
functionality, but don't be surprised if something you think should work causes
your computer to segfault.  **petard** aims to catch these things and throw
JavaScript errors, but we don't have very good coverage yet.  Please do submit
a ticket or pull request if there's a type of programmer error that could be
caught earlier and an error thrown.

one more warning: it is currently VERY leaky.  don't use in any program that
will be running very long.

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
Ubuntu where it's isntalled as `llvm-config-3.6`, you'll enter:

    > LLVM_CONFIG=llvm-config-3.6 npm run build

or

    > LLVM_CONFIG=llvm-config-3.6 npm install petard

Once you've got it built successfully, try this:

```coffeescript
llvm = require 'petard'

{i8, i32, pointerTo} = llvm.type

hello = llvm.CodeUnit "hello"

main = hello.makeFunction "main", i32
puts = hello.declareFunction "puts", i32, pointerTo i8

text = hello.constant "Hello, world!\n"

message = main.loadConstant text
main.callFunction puts, message

main.return 0

hello.writeBitcodeToFile "hello.bc"
```

Then use opt and clang as usual with the LLVM IR bitcode file.  Look at the
[example][21] folder for more examples.

Alternatively, you can JIT compile and call the function from JavaScript.

```coffeescript
hi = hello.makeFunction "hi", i32, pointerTo i8

prefix = hello.constant "Hello, "

hi.callFunction puts, hi.loadConstant prefix
hi.callFunction puts, hi.parameter 0

hi.return 0

greet = hi.jitCompile()

greet "Bill"
greet "Janice"
greet "Bart"
```

documentation
-------------

The underlying C++ API exposes the basics of LLVM IR construction, and the node
wrapper adds some higher-level functionality on top of that.  In the listing the
methods in JavaScript are marked with a *(js)*.

  * [petard types][15]
    * [Type][10]
    * [Value][11]
    * [Builder][12]
    * [FunctionBuilder][13]
    * [SwitchBuilder][18]
    * [CodeUnit][14]
  * [helpers][16]
    * [`type` dict][17]

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

##### [Type][10] value.type

A property containing the type of the value.

#### Builder

Something that can construct LLVM IR.  It represents a particular part of a
function (an LLVM block) as well as a cursor within that block where
instructions will be placed.  Calls to builder methods construct LLVM IR and
take and return values and other builders.

##### return(\[Number|[Value][11] value\])

Return from the function.  If a value is provided, it is the return value of the
function.  You must ensure that the value passed is of the same type as the
return type of the function.

##### [Value][11] parameter(Number index)

Reference function parameter `index`.

##### [Value][11] getElementPointer([Value][11] base, Number|[Value][11] indexList...)

Perform pointer math on the aggregate type pointer to produce a new pointer
to the given structure member.  For details, see the [GetElementPointer FAQ][2].

##### [Value][11] extractElement([Value][11] vec, Number|[Value][11] index)

Extract the element at the specified index from the vector.

##### [Value][11] insertElement([Value][11] vec, [Value][11] value, Number|[Value][11] index)

Insert the value as the element at the specified index from the vector.

##### [Value][11] loadConstant([Value][11] constant)

Load a constant value.  Currently the only usage is to load a string constant,
see the first example.

##### [Value][11] callFunction([FunctionBuilder][13]|[FunctionValue][20] fn, \[[Value][11] params...\])

Call the given internal or external function, passing in the parameter values.

##### [Value][11] alloca([Type][10] type, \[Number|[Value][11] arraySize\])

Allocate space the size of type on the stack.  If an arraySize is provided,
instead allocate that much space times the arraySize.  Returns a pointer value
to the space.

##### [Value][11] load([Value][11] pointer)

Load a value from the memory pointed to by pointer.

##### store([Value][11] value, [Value][11] pointer)

Store the value in the memory pointed to by pointer.

##### [Value][11] add([Value][11] left, [Value][11] right)

Add the values.  Works on integer and float values.

##### [Value][11] sub([Value][11] left, [Value][11] right)

Subtract the right value from the left.  For integers and floats.

##### [Value][11] mul([Value][11] left, [Value][11] right)

Multiply the values.  They can be integers or floats.

##### [Value][11] udiv([Value][11] left, [Value][11] right)

Unsigned integer divide the left value by the right.

##### [Value][11] sdiv([Value][11] left, [Value][11] right)

Signed integer divide the left value by the right.

##### [Value][11] fdiv([Value][11] left, [Value][11] right)

Floating point divide the left value by the right.

##### [Value][11] urem([Value][11] left, [Value][11] right)

The remainder when the left value is unsigned integer divided by the right.

##### [Value][11] srem([Value][11] left, [Value][11] right)

The remainder when the left value is signed integer divided by the right.

##### [Value][11] frem([Value][11] left, [Value][11] right)

The remainder when the left value is floating point divided by the right.

##### [Value][11] and([Value][11] left, [Value][11] right)

Bitwise and the values.

##### [Value][11] or([Value][11] left, [Value][11] right)

Bitwise or the values.

##### [Value][11] xor([Value][11] left, [Value][11] right)

Bitwise xor the values.

##### [Value][11] shl([Value][11] left, [Value][11] right)

Shift the left value left by the right value number of bits.

##### [Value][11] lshr([Value][11] left, [Value][11] right)

Shift the left value right logically (sign-ignoring) by the right value number
of bits.

##### [Value][11] ashr([Value][11] left, [Value][11] right)

Shift the left value right arithmetically (sign-preserving) by the right value
number of bits.

##### [Value][11] equal([Value][11] left, [Value][11] right)

Compare the values for equality.

##### [Value][11] notEqual([Value][11] left, [Value][11] right)

Compare the values for inequality.

##### [Value][11] uGreaterThan([Value][11] left, [Value][11] right)

Unsigned greater than comparison.

##### [Value][11] uAtLeast([Value][11] left, [Value][11] right)

Unsigned greater than or equal to comparison.

##### [Value][11] uLessThan([Value][11] left, [Value][11] right)

Unsigned less than comparison.

##### [Value][11] uAtMost([Value][11] left, [Value][11] right)

Unsigned less than or equal to comparison.

##### [Value][11] sGreaterThan([Value][11] left, [Value][11] right)

Signed greater than comparison.

##### [Value][11] sAtLeast([Value][11] left, [Value][11] right)

Signed greater than or equal to comparison.

##### [Value][11] sLessThan([Value][11] left, [Value][11] right)

Signed less than comparison.

##### [Value][11] sAtMost([Value][11] left, [Value][11] right)

Signed less than or equal to comparison.

##### [Value][11] foEqual([Value][11] left, [Value][11] right)

Float value ordered equality comparison.

##### [Value][11] foNotEqual([Value][11] left, [Value][11] right)

Float value ordered inequality comparison.

##### [Value][11] foGreaterThan([Value][11] left, [Value][11] right)

Float value ordered greater than comparison.

##### [Value][11] foAtLeast([Value][11] left, [Value][11] right)

Float value ordered greater than or equal to comparison.

##### [Value][11] foLessThan([Value][11] left, [Value][11] right)

Float value ordered less than comparison.

##### [Value][11] foAtMost([Value][11] left, [Value][11] right)

Float value ordered less than or equal to comparison.

##### [Value][11] fuEqual([Value][11] left, [Value][11] right)

Float value unordered equality comparison.

##### [Value][11] fuNotEqual([Value][11] left, [Value][11] right)

Float value unordered inequality comparison.

##### [Value][11] fuGreaterThan([Value][11] left, [Value][11] right)

Float value unordered greater than comparison.

##### [Value][11] fuAtLeast([Value][11] left, [Value][11] right)

Float value unordered greater than or equal to comparison.

##### [Value][11] fuLessThan([Value][11] left, [Value][11] right)

Float value unordered less than comparison.

##### [Value][11] fuAtMost([Value][11] left, [Value][11] right)

Float value unordered less than or equal to  comparison.

##### [Value][11] trunc([Value][11] value, [Type][10] type)

Truncate the integer value to a smaller type.

##### [Value][11] zext([Value][11] value, [Type][10] type)

Zero extend the integer value to a larger type.

##### [Value][11] sext([Value][11] value, [Type][10] type)

Sign extend the integer value to a larger type.

##### [Value][11] fpToUI([Value][11] value, [Type][10] type)

Convert the floating point value to an unsigned integer.

##### [Value][11] fpToSI([Value][11] value, [Type][10] type)

Convert the floating point value to a signed integer.

##### [Value][11] uiToFP([Value][11] value, [Type][10] type)

Convert the unsigned integer value to a floating point.

##### [Value][11] siToFP([Value][11] value, [Type][10] type)

Convert the signed integer value to a floating point.

##### [Value][11] fpTrunc([Value][11] value, [Type][10] type)

Truncate the floating point value to a smaller type.

##### [Value][11] fpext([Value][11] value, [Type][10] type)

Extend the floating point value to a larger type.

##### [Value][11] ptrToInt([Value][11] value, [Type][10] type)

Convert the pointer to an integer type.

##### [Value][11] intToPtr([Value][11] value, [Type][10] type)

Convert the integer to a pointer type.

##### [Value][11] bitcast([Value][11] value, [Type][10] type)

Cast the value to another type without changing the bits.

##### [Value][11] select([Value][11] condition, [Value][11] ifTrue, [Value][11] ifFalse)

Non-branching value select.  Returns the value corresponding to the ifTrue value
if the condition is true, and the ifFalse value if the condition is not true.
Also operates on vectors.

##### [Value][11] value([Type][10] type, Any value)

Construct a new LLVM value of the given type for the given JavaScript value.

##### {[Builder][12] then, [Builder][12] else} if([Value][11] condition) *(js)*

Builds the if conditional control structure for the given condition.  Returns an
object with then and else properties, each builders for the then and else blocks.

##### [Builder][12] while(BuilderCallback buildCondition) *(js)*

with [Value][11] BuilderCallback([Builder][12] condition)

Builds the while loop control structure.  The callback should take a builder for
the loop condition and should return the condition value.  Returns a builder for
the body of the loop.

##### br([Builder][12] target)

Unconditional branch to the target.

##### br([Value][11] condition, [Builder][12] ifTrue, [Builder][12] ifFalse)

Conditional branch.  If the condition is true, branch to the ifTrue builder,
otherwise branch to the ifFalse builder.

##### [SwitchBuilder][18] switch([Value][11] condition, [Builder][12] defaultDest)

Multi-way branch.  If none of the cases added to the returned switch builder
match the value of the condition, control follows the defaultDest.

##### [ChooseBuilder][19] choose([Value][11] condition) *(js)*

Builds the switch control structure.  While the `switch` method simply produces
a multi-way branch, this provides the high-level control you're expecting
out of the box, namely merging back to one place at the end.

##### [Builder][12] createBlock(String name)

Create another block in the same function.  Use with the branching instructions
to create custom control structures.  See the node wrapper for examples on
using `createBlock`, `splitBlock`, `useBlock`, `insertBefore`, `insertAfter`,
and `br` to build control structures.

##### [Builder][12] splitBlock(String name)

Split the current block at the cursor.  The object itself retains the top half
(all the instructions written _before_) and the returned builder takes the
bottom half (all the instructions written _after_).  In practice this usually
means the terminator of the block is the only instruction in the new block.

##### useBlock([Builder][12] target)

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

##### [Type][10] fnBuilder.type

A property containing the type of the function.

##### Function jitCompile() *(js)*

JIT compile the function, the set up a foreign-function interface to make the
function callable from JavaScript.  Uses the [ffi][70] library to dynamically
bind the functino pointer.

#### SwitchBuilder

A special tool for working with switch statements.  It is returned from the
[switch][22] method of a builder.

##### addCase(Number onVal, [Builder][12] target)

Adds a case for the value `onVal`, branching to target.

#### ChooseBuilder *(js)*

The same thing as the switch builder, but for [choose][23] statements.

##### [Builder][12] defaultCase

The default case builder.

##### [Builder][12] addCase(Number onVal)

Adds a case for the value `onVal`, returning a builder for the case.

#### CodeUnit

Represents an overall unit of code, such as a file.  Directly corresponds to an
LLVM Module.  It has a few helpful methods.

##### [Value][11] unit.constant([Value][11] value)

Compile a constant value into the IR.  At the moment the only use is to load a
constant string.  See the first example for usage.

##### [Value][11] unit.declareFunction(String name, \[[Type][10] returns\], \[[Type][10] takes...\])

Declare a function external to this code unit that will be linked in.  Takes the
name of the function and optionally the return type and parameter types.
Returns a special FunctionValue.

##### [FunctionBuilder][13] unit.makeFunction(String name, \[[Type][10] returns\], \[[Type][10] takes...\])

Create a new function in this code unit with the given name and optionall the
return type and parameter types.

##### unit.dump()

Dump the code unit IR to the screen.  Useful for debugging, not so much for
regular usage, since the underlying LLVM method always dumps to stderr.

##### unit.writeBitcodeToFile(String filename)

Write the bitcode for this code unit to a file with the given filename.  The
file must not exist, if it does this method will fail.

##### Buffer unit.jitFunction([FunctionBuilder][13] fn)

JIT compile the function.  Returns the raw pointer to the function as a Buffer.
You may find the corresponding helper method on [FunctionBuilder][13] to be more
useful since you can call the result of that method from JavaScript.

### helpers

#### `type` dict

This dict contains all the basic types for easy reference.

  * `i1` - A one bit integer (a boolean).
  * `i8` - A single byte.
  * `i16` - A small word.
  * `i32` - A medium word.
  * `i64` - A long word.
  * `f16` - A half-precision floating point value. (LLVM's `half`)
  * `f32` - A single-precision floating point value. (LLVM's `float`)
  * `f64` - A double-precision floating point value. (LLVM's `double`)
  * [Type][10] arrayOf(Number size, [Type][10] element) - Takes a size and type
    and returns an array of that size and type.
  * [Type][10] vectorOf(Number size, [Type][10] element) - Takes a size and type
    and returns a vector of that size and type.
  * [Type][10] structOf(\[[Type][10]\] elements) - Takes an array of types and
    returns a struct composed of those types.
  * [Type][10] pointerTo([Type][10] pointee) - Takes a type and returns a
    pointer to that type.

more information
----------------

  * [LLVM Language Reference][0]
  * [LLVM Programmer's Manual][1]

[0]: http://llvm.org/docs/LangRef.html
[1]: http://llvm.org/docs/ProgrammersManual.html
[2]: http://llvm.org/docs/GetElementPtr.html

[70]: https://www.npmjs.com/package/ffi

[10]: https://github.com/couchand/petard#type
[11]: https://github.com/couchand/petard#value
[12]: https://github.com/couchand/petard#builder
[13]: https://github.com/couchand/petard#functionbuilder
[14]: https://github.com/couchand/petard#codeunit
[15]: https://github.com/couchand/petard#petard-types
[16]: https://github.com/couchand/petard#helpers
[17]: https://github.com/couchand/petard#type-dict
[18]: https://github.com/couchand/petard#switchbuilder
[19]: https://github.com/couchand/petard#choosebuilder-js

[20]: https://github.com/couchand/petard#value-unitdeclarefunctionstring-name-type-returns-type-takes
[21]: https://github.com/couchand/petard/tree/master/example
[22]: https://github.com/couchand/petard#switchbuilder-switchvalue-condition-builder-defaultdest
[23]: https://github.com/couchand/petard#choosebuilder-choosevalue-condition-js

[90]: https://github.com/couchand/petard#introduction
[91]: https://github.com/couchand/petard#dependencies
[92]: https://github.com/couchand/petard#getting-started
[93]: https://github.com/couchand/petard#documentation
[94]: https://github.com/couchand/petard#more-information

##### ╭╮☲☲☲╭╮ #####
