VERSION		= 2.0.9
OUT			= d3m0n_sdk_$(VERSION).a
OBJ_DIR		= obj
CXX			= d3c
AR			= arm-none-eabi-ar
C_FILES		= $(shell find . -type f -name '*.c')
CPP_FILES	= $(shell find . -type f -name '*.cpp')
ASM_FILES	= $(shell find . -type f \( -name '*.s' -o -name '*.S' \))
O_FILES		= $(C_FILES:%.c=$(OBJ_DIR)/%.o) \
			  $(CPP_FILES:%.cpp=$(OBJ_DIR)/%.o) \
			  $(patsubst %.s,$(OBJ_DIR)/asm/%.o,$(filter %.s,$(ASM_FILES))) \
			  $(patsubst %.S,$(OBJ_DIR)/asm/%.o,$(filter %.S,$(ASM_FILES)))
D_FILES		= $(O_FILES:.o=.d)
DEBUG		= 0
DEBUG_ALLOC = 0
C_FLAGS		= -Wall -Wextra -Werror -DVERSION="$(VERSION)" -DDEBUG=$(DEBUG) -DDEBUG_ALLOC=$(DEBUG_ALLOC) -Ilib -MMD -MP

# only used in copy_headers
COPY_DEST	?= "/tmp"

all: $(OUT)

$(OUT): $(O_FILES)
	@mkdir -p "$(OBJ_DIR)"
	@rm -f "$(OUT)"
	@$(AR) rcs "$(OUT)" $^

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p "$(@D)"
	@"$(CXX)" -c "$<" -o "$@" $(C_FLAGS)
	@printf "\033[35m  => $<\033[0m\n"

$(OBJ_DIR)/%.o: %.c
	@mkdir -p "$(@D)"
	@"$(CXX)" -c "$<" -o "$@" $(C_FLAGS)
	@printf "\033[35m  => $<\033[0m\n"

$(OBJ_DIR)/asm/%.o: %.s
	@mkdir -p "$(@D)"
	@"$(CXX)" -c "$<" -o "$@" $(C_FLAGS)
	@printf "\033[35m  => $<\033[0m\n"

$(OBJ_DIR)/asm/%.o: %.S
	@mkdir -p "$(@D)"
	@"$(CXX)" -c "$<" -o "$@" $(C_FLAGS)
	@printf "\033[35m  => $<\033[0m\n"

copy_headers:
	@mkdir -p "$(COPY_DEST)"
	cp -r ./lib/. "$(COPY_DEST)/"

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(OUT)

re: fclean all

-include $(D_FILES)

.PHONY: all re clean fclean copy_headers
