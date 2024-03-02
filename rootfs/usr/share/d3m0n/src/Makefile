SRC=*.cpp *.c 
OUT=d3m0n
CFLAGS=-Ilib  -g -Og -std=c++17 -lstdc++
WARNINGS_IGNORE=-Wno-unused-result -Wno-unused-variable -Wno-format-security -Wno-write-strings
GCC=g++


all:	$(OUT)

$(OUT):	$(SRC)
	$(GCC) $(WARNINGS_IGNORE) $(CFLAGS) -o $@ $?

debug:
	gdb $(OUT)

clean:
	$(RM) $(OUT)