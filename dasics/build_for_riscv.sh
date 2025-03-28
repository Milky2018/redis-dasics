#!/bin/sh

HOME=$(pwd)/..

export CC=riscv64-linux-gnu-gcc
export AR=riscv64-linux-gnu-ar
export RANLIB=riscv64-linux-gnu-ranlib
export STRIP=riscv64-linux-gnu-strip

git clone https://github.com/DASICS-ICT/LibDASICS.git
cd LibDASICS
make -j 10

DASICS_A = $HOME/LibDASICS/build/libdasics.a
DASICS_H = $HOME/LibDASICS/include

cd $HOME/src
make -j 10