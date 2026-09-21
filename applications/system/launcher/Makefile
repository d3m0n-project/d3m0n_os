CXX         = d3c

CXXFLAGS    = -Wall -Wextra -Werror

SRC_DIR     = src
OBJ_DIR     = obj

CPP_FILES   = $(shell find $(SRC_DIR) -type f -name '*.cpp')
O_FILES     = $(CPP_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
D_FILES     = $(O_FILES:.o=.d)

TMP_NAME    = $(OBJ_DIR)/tmp.d3m0n

APP_PATH    ?=

define APP_META_NAME
$(shell \
	tmp=$$(mktemp); \
	objcopy --dump-section .appmeta=$$tmp "$(1)" 2>/dev/null && \
	dd if=$$tmp bs=1 skip=64 status=none | tr -d '\0'; \
	rm -f $$tmp \
)
endef

APP_NAME    := $(call APP_META_NAME,$(TMP_NAME))
OUT         := $(OBJ_DIR)/$(APP_NAME).d3m0n


export: $(OUT)
	@printf "   \033[35m=> APP_PATH: $(APP_PATH)\033[0m\n" >&2
	@printf "   \033[35m=> APP:      $(OUT)\033[0m\n" >&2


$(OUT): $(TMP_NAME)
	@mkdir -p "$(APP_PATH)"
	@cp "$<" "$(APP_PATH)/$(notdir $@)"


$(TMP_NAME): $(O_FILES)
	@mkdir -p "$(OBJ_DIR)"
	$(CXX) $(CXXFLAGS) $^ -o $@


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p "$(OBJ_DIR)"
	$(CXX) $(CXXFLAGS) -MMD -MP -c "$<" -o "$@"


clean:
	rm -rf $(OBJ_DIR)


fclean: clean


re: fclean export


.PHONY: clean fclean re export

-include $(D_FILES)