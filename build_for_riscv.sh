#!/bin/sh

export CC=riscv64-linux-gnu-gcc
export AR=riscv64-linux-gnu-ar
export RANLIB=riscv64-linux-gnu-ranlib
export STRIP=riscv64-linux-gnu-strip

make -j 10