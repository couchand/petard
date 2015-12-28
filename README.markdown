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

Installing things may be tricky, since it relies on getting everything
right with LLVM.  Once you've done that successfully, try this:

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

*That would be nice.*

more information
----------------

  * [LLVM Programmer's Manual][0]

[0]: http://llvm.org/docs/ProgrammersManual.html

##### ╭╮☲☲☲╭╮ #####
