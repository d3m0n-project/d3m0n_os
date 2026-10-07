CXX         = d3c
APP_PATH    ?= ./
PWD         = $(shell pwd)
OUT         = $(APP_PATH)$(shell basename $(PWD)).d3m0n

CXXFLAGS    = -Wall -Wextra -Werror

SRC_DIR     = src
OBJ_DIR     = obj

CPP_FILES   = $(shell find $(SRC_DIR) -type f -name '*.cpp')
O_FILES     = $(CPP_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
D_FILES     = $(O_FILES:.o=.d)


RESET       = \033[0m
BOLD        = \033[1m
DIM         = \033[2m

PURPLE      = \033[35m
CYAN        = \033[36m
GREEN       = \033[32m
YELLOW      = \033[33m
RED         = \033[31m
WHITE       = \033[37m



export: $(OUT)
	@printf "\n" >&2
	@printf "$(BOLD)$(PURPLE)  d3c compilation$(RESET)\n" >&2
	@printf "$(DIM)  ─────────────────────────────────────────────$(RESET)\n" >&2
	@printf "  $(CYAN)APP PATH$(RESET)  $(APP_PATH)\n" >&2
	@printf "  $(CYAN)OUTPUT$(RESET)    $(OUT)\n" >&2
	@printf "$(DIM)  ─────────────────────────────────────────────$(RESET)\n" >&2
	@printf "  $(GREEN)✓ Build complete$(RESET)\n\n" >&2



$(OUT): $(O_FILES)
	@printf "  $(PURPLE)$(BOLD)LINK$(RESET)  $(OUT)\n" >&2
	@mkdir -p "$(OBJ_DIR)"
	@mkdir -p "$(APP_PATH)"
	$(CXX) $(CXXFLAGS) $^ -o $@ >&2
	@printf "  $(GREEN)✓$(RESET) $(OUT)\n" >&2



$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p "$(OBJ_DIR)"
	@printf "  $(CYAN)CXX$(RESET)   %s\n" "$<" >&2
	$(CXX) $(CXXFLAGS) -MMD -MP -c "$<" -o "$@" >&2
	@printf "  $(GREEN)✓$(RESET)      %s\n" "$@" >&2



clean:
	@printf "  $(YELLOW)CLEAN$(RESET)  $(OBJ_DIR)\n"  >&2
	@rm -rf "$(OBJ_DIR)"

fclean: clean
	@printf "  $(YELLOW)CLEAN$(RESET)  $(OUT)\n" >&2
	@rm -f "$(OUT)"

re: fclean export


.PHONY: clean fclean re export

-include $(D_FILES)