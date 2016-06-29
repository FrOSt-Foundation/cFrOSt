# cFrOSt


FrOSt is a free operating system under the free license GNU GPL, made by a tiny group of french people. We worked hard on it, but now we decided to work harder and do it also in C using the llvm-dcpu16 backend.


## What is cFrOSt?

cFrOSt is a community French project which goal is to create a free and Open-Source OS for ~~0x10^c~~ [techcompliant](https://reddit.com/r/techcompliant).


## How to run cFrOSt?

You need to install [krasin/llvm-dcpu16](https://github.com/krasin/llvm-dcpu16/) and [binutils-dcpu16](https://github.com/frot/binutils-dcpu16). Further steps will be added to this README as we get used to them.

## How can I develop a program for that OS?

The wiki contains all the information you need.


## How to contribute

### Development environment

We use git as a CVS (obviously).

You can clone the repository and install llvm-dcpu16 and binutils-dcpu16 (as described in the "How to run cFrOSt?" section), then use your favorite C IDE and compile using the command line.


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
