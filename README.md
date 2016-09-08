# cFrOSt


FrOSt is a free operating system under the free license GNU GPL, made by a tiny group of french people. We are working hard on it, but  we decided to work harder and do it also in C using the llvm-dcpu16 backend.


## What is cFrOSt?

cFrOSt is a community French project which goal is to create a free and Open-Source OS for ~~0x10^c~~ [techcompliant](https://reddit.com/r/techcompliant).


## How to run cFrOSt?

### Windows

* You need to download [llvm-dcpu16/llvm-dcpu16](https://github.com/llvm-dcpu16/llvm-dcpu16/) (you can download their sdk, complete with [binutils-dcpu16](https://github.com/frot/binutils-dcpu16), [here](https://github.com/llvm-dcpu16/llvm-dcpu16/downloads)), [yamakaky/dcpu](https://github.com/yamakaky/dcpu) and [DCPU-Toolchain-cli](https://github.com/azertyfun/DCPU-Toolchain/releases/download/v1.4.2/DCPU-Toolchain-cli.zip);
* Extract the bin folder from the sdk here, or copy the binaries you compiled in a tools/ folder here. You should now have cFrOSt/tools/clang.exe;
* Install rust and build the assembler part of yamakaky/dcpu, and copy target/release/assembler.exe to the tools/ folder;
* Extract DCPU-Toolchain-cli.zip in tools/, so you have tools/DCPU-Toolchain.jar
* Create an assembly file with src/include.c, and all \*.dasm files into one file. Compile it by running `bin/clang.exe file.c` then `bin/assembler.exe file.s -o FrOSt.bin` then `move FrOSt.bin tools/`, or if you have cygwin just run `make` then `make run`.
* If you do not have cygwin, run the project with `cd tools` then `java -jar DCPU-Toolchain.jar java -jar DCPU-Toolchain.jar run none --rom=FrOSt.bin --clock --keyboard --lem1802 --lem1802-fps=10 --debugger`

### Linux x86 or x86_64

* Run `git clone https://github.com/azertyfun/DCPU-Toolchain --depth=1` to download cFrOSt ;
* Run `cd tools && ./download-toolchain.sh` to download DCPU-Toolchain ;
* Run `make`, then `make run`.

Useful env variables:

* AS: the assembler to use
* CC: the C compiler to use (must support the DCPU)
* CFLAGS, CPPFLAGS, ASFLAGS: pass custom arguments

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
