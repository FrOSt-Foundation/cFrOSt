#!/usr/bin/env bash

wget --quiet --show-progress --continue \
    https://github.com/azertyfun/DCPU-Toolchain/releases/download/v1.4.4/DCPU-Toolchain-cli.zip \
    https://github.com/Yamakaky/dcpu/releases/download/0.5.0/{assembler,emulator}
unzip -q -u -o DCPU-Toolchain-cli.zip
chmod +x {assembler,emulator}
