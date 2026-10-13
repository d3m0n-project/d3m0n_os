OUT			= test_program
CC			= d3c
C_FILES		= main.c
O_FILES		= $(C_FILES:.c=.o)


all: $(OUT)

$(OUT): $(O_FILES)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $^ -o $@

clean:
	rm -rf $(O_FILES)

fclean: clean
	rm -rf $(OUT)

re: fclean all

export: all
	mv $(OUT) ../bin/

re_export: fclean export

.PHONY: all re clean fclean export re_export