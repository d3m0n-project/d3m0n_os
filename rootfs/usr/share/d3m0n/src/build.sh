#!/bin/sh

clear

make clean
make

#gcc *.cpp *.c -Ilibs/ili9340/ -Wall -std=c99 -o d3m0n | grep "error"

chmod 777 ./d3m0n

./d3m0n