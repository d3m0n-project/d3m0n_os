OUT						= d3c
CXX						= g++
CPP_FILES				= d3c.cpp
SRC_DIR					= src
OBJ_DIR					= obj
SDK_BUILD				= $(OBJ_DIR)/sdk.a
O_FILES					= $(CPP_FILES:%.cpp=$(OBJ_DIR)/%.o)
D_FILES					= $(CPP_FILES:%.cpp=$(OBJ_DIR)/%.d)
INSTALL_DIR_ROOT		= /opt/com.ares/
INSTALL_DIR_USER		= $(HOME)/.local/bin
# install for current user only or full system
INSTALL_FOR_EVERYONE	= 0
# make d3c contain sdk or not
INCLUDE_SDK				?= 1
CPP_FLAGS				= -Wall -Wextra -Werror -DINCLUDE_SDK=$(INCLUDE_SDK) -MMD -MP
CWD						= $(shell pwd)


ifeq ($(INSTALL_FOR_EVERYONE),1)
INSTALL_DIR				:= $(INSTALL_DIR_ROOT)
else
INSTALL_DIR				:= $(INSTALL_DIR_USER)
endif

all: $(SDK_BUILD) install

$(SDK_BUILD):
# compile a d3c without sdk
	@$(MAKE) build \
		OUT="$(OBJ_DIR)/tmp_d3c" \
		INCLUDE_SDK=0
	@echo "\033[35mFinished compiling temp d3c!\033[0m"

	@$(MAKE) -C sdk \
		CXX="$(CWD)/$(OBJ_DIR)/tmp_d3c" \
		OUT="$(CWD)/$(SDK_BUILD)"
	@echo "\033[35mFinished compiling sdk!\033[0m"

	@rm -rf $(O_FILES)
	@echo "\033[35mCleaned up temp d3c files!\033[0m"

build: $(OUT)

$(OUT): $(O_FILES)
	@mkdir -p $(OBJ_DIR)
ifneq ($(INCLUDE_SDK),0)
	ld -r -b binary -o "$(OBJ_DIR)/sdk.o" "$(SDK_BUILD)"
	$(CXX) "$(OBJ_DIR)/sdk.o" $^ -o $@ $(CPP_FLAGS)
else
	$(CXX) $^ -o $@ $(CPP_FLAGS)
endif
	@echo "\033[35mLinked $(OUT)!\033[0m"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) -c $< -o $@ $(CPP_FLAGS)

install: build
	@echo -n "Install $(OUT) to $(INSTALL_DIR)? [Y/n] "; \
	read answer; \
	case "$$answer" in \
		[nN]|[nN][oO]) \
			echo "Installation cancelled."; \
			exit 1; \
			;; \
		*) \
			;; \
	esac

	@mkdir -p "$(INSTALL_DIR)"

ifeq ($(INSTALL_FOR_EVERYONE),1)
	@install -m 755 "$(OUT)" "$(INSTALL_DIR)/$(OUT)"
else
	@install -m 755 "$(OUT)" "$(INSTALL_DIR)/$(OUT)"
endif

	@echo
	@echo "\033[32mSUCCESS\033[0m: $(OUT) installed to $(INSTALL_DIR)/$(OUT)"
	@echo
	@echo "$(MAKE) sure $(INSTALL_DIR) is in your PATH."


remove:
ifeq ($(INSTALL_FOR_EVERYONE),1)
	@sudo rm -f "$(INSTALL_DIR)/$(OUT)"
else
	@rm -f "$(INSTALL_DIR)/$(OUT)"
endif
	@echo "\033[32mSUCCESS\033[0m: $(OUT) removed."

clean:
	rm -rf $(OBJ_DIR)
	@echo "\033[35mCleaning SDK\033[0m"
	@$(MAKE) -C sdk clean

fclean: clean
	rm -rf $(OUT)
	@echo "\033[35mFull Cleaning SDK\033[0m"
	@$(MAKE) -C sdk fclean

re: fclean all


.PHONY: all clean fclean re install remove build

-include $(D_FILES)