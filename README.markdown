petard
======

a js llvm library

        For 'tis the sport to have the enginer
        Hoist with his own petar'...

                    - Hamlet, Shakespeare

  * introduction
  * getting started
  * documentation
  * more information

introduction
------------

Frustrated with the state of JS support for LLVM, I built another library.

getting started
---------------

Installing things may be tricky, since it relies on getting everything
right with LLVM.  Once you've done that successfully, try this:

```coffeescript
llvm = require 'petard'

hello = llvm.createModule "hello"
main = hello.createFunction "main", [], llvm.type.int

text = hello.constant "Hello, world!\n"

main.loadConstant text
main.callFunction "puts"

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

[0][http://llvm.org/docs/ProgrammersManual.html]

##### ╭╮☲☲☲╭╮ #####
