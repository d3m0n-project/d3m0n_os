#!/bin/sh

gcc *.cpp *.c -Ilibs/ili9340/ -Wall -std=c99 -o d3m0n

chmod 777 ./d3m0n

./d3m0n