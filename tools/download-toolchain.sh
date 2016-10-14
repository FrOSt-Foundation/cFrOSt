#!/usr/bin/env bash

wget --no-verbose --show-progress --continue \
    https://github.com/azertyfun/DCPU-Toolchain/releases/download/v1.4.3/DCPU-Toolchain-cli.zip \
    https://github.com/Yamakaky/dcpu/releases/download/0.4.0/{assembler,emulator}
unzip -o DCPU-Toolchain-cli.zip
chmod +x {assembler,emulator}
