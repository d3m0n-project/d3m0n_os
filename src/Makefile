CC			= gcc
C_FLAGS		= -Wall -Wextra -Werror
LD_FLAGS	= -g
C_FILES		= main.c
O_FILES		= $(C_FILES:.c=.o)
OUT			= d3m0n_os

all: $(OUT)
	
$(OUT): $(O_FILES)
	$(CC) $(C_FLAGS) $^ -o $@ $(LD_FLAGS)

clean:
	rm -rf $(OUT) $(O_FILES)

.PHONY: all clean