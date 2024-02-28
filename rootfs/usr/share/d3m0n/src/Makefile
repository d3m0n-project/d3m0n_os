SRC=*.cpp *.c 
OUT=d3m0n
CFLAGS=-Ilib -g -Og -std=c++17 -lstdc++
GCC=g++


all:	$(OUT)

$(OUT):	$(SRC)
	$(GCC) $(CFLAGS) -o $@ $?

debug:
	gdb $(OUT)

clean:
	$(RM) $(OUT)