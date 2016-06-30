# cFrOSt


FrOSt is a free operating system under the free license GNU GPL, made by a tiny group of french people. We are working hard on it, but  we decided to work harder and do it also in C using the llvm-dcpu16 backend.


## What is cFrOSt?

cFrOSt is a community French project which goal is to create a free and Open-Source OS for ~~0x10^c~~ [techcompliant](https://reddit.com/r/techcompliant).


## How to run cFrOSt?

### Windows

* You need to download [llvm-dcpu16/llvm-dcpu16](https://github.com/llvm-dcpu16/llvm-dcpu16/) (you can download their sdk, complete with [binutils-dcpu16](https://github.com/frot/binutils-dcpu16), [here](https://github.com/llvm-dcpu16/llvm-dcpu16/downloads)) and [yamakaky/dcpu](https://github.com/yamakaky/dcpu);
* Extract the bin folder from the sdk here, or copy the binaries you compiled in a bin/ folder here. You should now have cFrOSt/bin/clang.exe;
* Install rust and build the assembler part of yamakaky/dcpu, and copy target/release/assembler.exe to the bin/ folder;
* Compile by runing `bin/clang.exe include.c` then `bin/assembler.exe include.s` then `move FrOSt.bin tools/`, or if you have cygwin just run `make` then `make run`.

### Linux x86 or x86_64

* Run `make`, then `make run`.

## How can I develop a program for that OS?

The wiki contains all the information you need.


## How to contribute

### Development environment

We use git as a CVS (obviously).

You can use your favorite editor and then run the project as explained in the "How to run cFrOSt?" section.

### Syntax guidelines

#### General rules

* We use [waffle](https://waffle.io/FrOSt-Foundation/cFrOSt) to manage our project;
* git handles EOL character(s) (LF/CRLF), so no problem with that;
* __Document__ your code ! It will be easier to maintain and understand later;
* Write simple and clear commit messages : One change per commit. For instance, 'Misc driver changes' can surely be split in multiple commits;
* Don't be afraid to use `git branch` for any change of more than one commit (cf. precedent rule);
* Don't be afraid to read and check other people's commits. The code will only be better!


#### Norms

* All rules **must be respected** an can be found on the [wiki](https://github.com/FrOSt-Foundation/cFrOSt/wiki/Coding-rules).

### Available branches :

* `master` : It's the main branch and is only modified for project releases.
* `indev` : It's the development branch and must be modified only by people who know it.  /!\ May not work, and will be more or less buggy anyways.
* Other temporary branches may appear (cf. general rules).

## Useful links

* [Latest hardware specifications](https://github.com/techcompliant/TC-Specs);
* [Old DCPU specifications](https://web.archive.org/web/20130817110245/http://dcpu.com).
