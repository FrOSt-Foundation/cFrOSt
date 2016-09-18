#!/bin/sh
java -jar DCPU-Toolchain.jar run ../bin/FrOSt.bin --bootloader=../bin/FrOSt_header.bin --clock --keyboard --lem1802 --lem1802-fps=10 --debugger
