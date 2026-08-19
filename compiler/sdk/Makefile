VERSION		= 2.0.9
OUT			= d3m0n_sdk_$(VERSION).a
OBJ_DIR		= obj
CXX			= d3c
AR			= arm-none-eabi-ar
C_FILES		= $(shell find . -name '*.c')
CPP_FILES	= $(shell find . -name '*.cpp')
O_FILES		= $(C_FILES:%.c=$(OBJ_DIR)/%.o) $(CPP_FILES:%.cpp=$(OBJ_DIR)/%.o)
C_FLAGS		= -Wall -Wextra -Werror -DVERSION="$(VERSION)" -Ilib

all: $(OUT)

$(OUT): $(O_FILES)
	@mkdir -p "$(OBJ_DIR)"
	@$(AR) rcs "$(OUT)" $^

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	@"$(CXX)" -c "$<" -o "$@" $(C_FLAGS)
	@echo "\033[35m  => $<\033[0m"

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	@"$(CXX)" -c "$<" -o "$@" $(C_FLAGS)
	@echo "\033[35m  => $<\033[0m"

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(OUT)

re: fclean all

.PHONY: all re clean fclean