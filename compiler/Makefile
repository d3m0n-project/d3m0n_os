OUT						= d3c
CXX						= g++
CPP_FILES				= d3c.cpp
SRC_DIR					= src
OBJ_DIR					= obj
SDK_BUILD				= $(OBJ_DIR)/sdk.a
O_FILES					= $(CPP_FILES:%.cpp=$(OBJ_DIR)/%.o)
D_FILES					= $(CPP_FILES:%.cpp=$(OBJ_DIR)/%.d)
INSTALL_DIR_ROOT		= /opt/com.ares
INSTALL_DIR_USER		= $(HOME)/com.ares
LIB_INSTALL_DIR			= /
# install for current user only or full system
INSTALL_FOR_EVERYONE	= 0
# make d3c contain sdk or not
INCLUDE_SDK				?= 1

ifeq ($(INSTALL_FOR_EVERYONE),1)
INSTALL_DIR				:= $(INSTALL_DIR_ROOT)
else
INSTALL_DIR				:= $(INSTALL_DIR_USER)
endif

SDK_HEADER_PATH			= $(INSTALL_DIR)/sdk
CPP_FLAGS				= -Wall -Wextra -Werror \
 							-DINCLUDE_SDK=$(INCLUDE_SDK) \
							-DLIB_PATH="\"$(SDK_HEADER_PATH)\"" \
							-MMD -MP
CWD						= $(shell pwd)
OBJCOPY					= $(shell $(CXX) -print-prog-name=objcopy)
TARGET_BFD				= $(shell $(OBJCOPY) --info | awk '/^elf/ {print $$1; exit}')




all: $(SDK_BUILD) install

$(SDK_BUILD):
	@mkdir -p $(SDK_HEADER_PATH)
# compile a d3c without sdk
	@$(MAKE) build \
		OUT="$(OBJ_DIR)/tmp_d3c" \
		INCLUDE_SDK=0
	@printf "\033[35mFinished compiling temp d3c!\033[0m\n"

	@$(MAKE) -C sdk \
		CXX="$(CWD)/$(OBJ_DIR)/tmp_d3c" \
		OUT="$(CWD)/$(SDK_BUILD)"
	@printf "\033[35mFinished compiling sdk!\033[0m\n"

	@make -C sdk copy_headers COPY_DEST="$(SDK_HEADER_PATH)"
	@printf "\033[35mCopied sdk headers!\033[0m\n"

	@rm -rf $(O_FILES)
	@printf "\033[35mCleaned up temp d3c files!\033[0m\n"

build: $(OUT)

$(OUT): $(O_FILES)
	@mkdir -p $(OBJ_DIR)
ifneq ($(INCLUDE_SDK),0)
	ld -r -b binary -o "$(OBJ_DIR)/linker.o" "linker.ld"
	$(OBJCOPY) -I binary -O $(TARGET_BFD) "$(SDK_BUILD)" "$(OBJ_DIR)/sdk.o"
	$(CXX) "$(OBJ_DIR)/sdk.o" "$(OBJ_DIR)/linker.o" $(O_FILES) -o $@ $(CPP_FLAGS)
else
	$(CXX) $(O_FILES) -o $@ $(CPP_FLAGS)
endif
	@printf "\033[35mLinked $(OUT)!\033[0m\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) -c $< -o $@ $(CPP_FLAGS)

install: build
	@mkdir -p $(INSTALL_DIR)
	@printf "Install $(OUT) to $(INSTALL_DIR)? [Y/n] "; \
	read answer; \
	case "$$answer" in \
		[nN]|[nN][oO]) \
			printf "\033[31mInstallation cancelled.\033[0m\n"; \
			exit 1; \
			;; \
		*) \
			;; \
	esac

	@mkdir -p "$(INSTALL_DIR)"
	@install -m 755 "$(OUT)" "$(INSTALL_DIR)/$(OUT)"

ifeq ($(INSTALL_FOR_EVERYONE),1)
	@ln -s "$(INSTALL_DIR)/$(OUT)" /bin/d3c                 2>/dev/null || true
else
	@ln -s "$(INSTALL_DIR)/$(OUT)" $(HOME)/.local/bin/d3c   2>/dev/null || true
endif

	@printf "\n"
	@printf "=============================================================\n"
	@printf "\033[32mSUCCESS\033[0m: $(OUT) installed to $(INSTALL_DIR)/$(OUT)\n"
	@printf "Make sure $(INSTALL_DIR) is in your PATH!\n"
	@printf "Put this command in your bashrc: \033[35mexport PATH=\"\$$PATH:$(INSTALL_DIR)\"\033[0m\n"
	@printf "=============================================================\n\n"


remove:
ifeq ($(INSTALL_FOR_EVERYONE),1)
	@sudo rm -f "$(INSTALL_DIR)/$(OUT)"
else
	@rm -f "$(INSTALL_DIR)/$(OUT)"
endif
	@printf "\033[32mSUCCESS\033[0m: $(OUT) removed.\n"

clean:
	rm -rf $(OBJ_DIR)
	@printf "\033[35mCleaning SDK\033[0m\n"
	@$(MAKE) -C sdk clean

fclean: clean
	rm -rf $(OUT)
	@printf "\033[35mFull Cleaning SDK\033[0m\n"
	@$(MAKE) -C sdk fclean

re: fclean all


.PHONY: all clean fclean re install remove build

-include $(D_FILES)