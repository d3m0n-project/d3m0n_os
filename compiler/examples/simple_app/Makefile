OUT			= test_app
CXX			= d3c
CXXFLAGS	= -Wall -Wextra -Werror

SRC_DIR		= src
OBJ_DIR		= obj
CPP_FILES	= $(shell find $(SRC_DIR) -type f -name '*.cpp')
O_FILES		= $(CPP_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
D_FILES		= $(O_FILES:.o=.d)

all: $(OUT)

$(OUT): $(O_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p "$(OBJ_DIR)"
	$(CXX) $(CXXFLAGS) -MMD -MP -c "$<" -o "$@"


clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(OUT)

re: fclean all

.PHONY: all clean fclean re

-include $(DEP_FILES)