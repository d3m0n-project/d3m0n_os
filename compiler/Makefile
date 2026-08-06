OUT			= d3s
CXX			= g++
CPP_FLAGS	= -Wall -Wextra -Werror
CPP_FILES	= app.cpp d3c.cpp utils.cpp
SRC_DIR		= src
OBJ_DIR		= obj
O_FILES		= $(CPP_FILES:%.cpp=$(OBJ_DIR)/%.o)
D_FILES		= $(CPP_FILES:%.cpp=$(OBJ_DIR)/%.d)

all: $(OUT)

$(OUT): $(O_FILES)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $^ -o $@ $(CPP_FLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) -c $< -o $@ $(CPP_FLAGS)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(OUT)

re: fclean all


.PHONY: all clean fclean re