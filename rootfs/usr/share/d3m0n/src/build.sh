#!/bin/sh

# solved bcm2835.h error by addind static_cast<uint32_t*>( void* )

clear

cd ./test
./build.sh
cd ..


# setting lib var
# LD_LIBRARY_PATH=$(pwd)/lib

make clean
make

#gcc *.cpp *.c -Ilibs/ili9340/ -Wall -std=c99 -o d3m0n | grep "error"

sudo chmod 777 ./d3m0n

sudo ./d3m0n