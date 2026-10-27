OUT						= d3c
CXX						= g++
CPP_FILES				= d3c.cpp \
						  vc4asm.cpp
SRC_DIR					= src
OBJ_DIR					= obj
TEMP_OBJ_DIR			= $(OBJ_DIR)/tmp
TEMP_D3C				= $(TEMP_OBJ_DIR)/d3c
SDK_BUILD				= $(OBJ_DIR)/sdk.a
SDK_FILES				= $(shell find sdk -type f \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' -o -name '*.s' -o -name '*.S' \))
TEMP_D3C_INPUTS		= $(shell find src vc4asm/src -type f \( -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \)) linker.ld vc4asm/share/vc4inc/vc4.qinc
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
CPP_FLAGS				= -Wall -Wextra -Werror -DINCLUDE_SDK=$(INCLUDE_SDK) \
							-DLIB_PATH="\"$(INSTALL_DIR)\"" \
							-MMD -MP
CWD						= $(shell pwd)
OBJCOPY					= $(shell $(CXX) -print-prog-name=objcopy)
TARGET_BFD				= $(shell $(OBJCOPY) --info | awk '/^elf/ {print $$1; exit}')


vc4asm_SRC_DIR			= ./vc4asm/src
vc4asm_OBJ_DIR			= $(OBJ_DIR)/vc4asm
VC4_QINC_OBJ			= $(OBJ_DIR)/vc4qinc.o
vc4asm_CPP_FILES		= expr.cpp Inst.cpp Validator.cpp ReadBin.cpp WriteC.cpp \
							ReadX.cpp Message.cpp WriteQasm.cpp utils.cpp WriteELF.cpp \
							ReaderBase.cpp Disassembler.cpp AssembleInst.cpp Parser.cpp \
							Eval.cpp WriterBase.cpp
vc4asm_O_FILES			= $(addprefix $(vc4asm_OBJ_DIR)/,$(vc4asm_CPP_FILES:.cpp=.o))
vc4asm_CXX_FLAGS		= -I$(vc4asm_SRC_DIR)


all: install


$(TEMP_D3C): $(TEMP_D3C_INPUTS)
	@$(MAKE) build OUT="$(TEMP_D3C)" OBJ_DIR="$(TEMP_OBJ_DIR)" INCLUDE_SDK=0
	@printf "\033[35mFinished compiling temporary d3c!\033[0m\n"

$(SDK_BUILD): $(TEMP_D3C) $(SDK_FILES)
	@$(MAKE) -C sdk CXX="$(CWD)/$(TEMP_D3C)" OUT="$(CWD)/$(SDK_BUILD)"
	@touch "$@"
	@printf "\033[35mFinished compiling SDK!\033[0m\n"

sdk_build: $(SDK_BUILD)

sdk_headers:
	@rm -rf "$(SDK_HEADER_PATH)"
	@mkdir -p "$(SDK_HEADER_PATH)"
	@cp -r sdk/lib/. "$(SDK_HEADER_PATH)/"
	@printf "\033[35mCopied SDK headers!\033[0m\n"

build: $(OUT)

ifeq ($(INCLUDE_SDK),0)
$(OUT): $(vc4asm_O_FILES) $(O_FILES) $(VC4_QINC_OBJ)
else
$(OUT): $(vc4asm_O_FILES) $(O_FILES) $(VC4_QINC_OBJ) $(SDK_BUILD)
endif
	@mkdir -p $(OBJ_DIR)
ifneq ($(INCLUDE_SDK),0)
	ld -r -b binary -o "$(OBJ_DIR)/linker.o" "linker.ld"
	$(OBJCOPY) -I binary -O $(TARGET_BFD) "$(SDK_BUILD)" "$(OBJ_DIR)/sdk.o"
	$(CXX) "$(OBJ_DIR)/sdk.o" "$(OBJ_DIR)/linker.o" $(VC4_QINC_OBJ) $(vc4asm_O_FILES) $(O_FILES) -o $@ $(CPP_FLAGS) $(vc4asm_CXX_FLAGS)
else
	$(CXX) $(vc4asm_O_FILES) $(O_FILES) -o $@ $(CPP_FLAGS) $(vc4asm_CXX_FLAGS)
endif
	@printf "\033[35mLinked $(OUT)!\033[0m\n"



$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) -c $< -o $@ $(CPP_FLAGS) -I$(vc4asm_SRC_DIR)

$(VC4_QINC_OBJ): vc4asm/share/vc4inc/vc4.qinc
	@mkdir -p $(OBJ_DIR)
	cd vc4asm/share/vc4inc && ld -r -b binary -o ../../../$(VC4_QINC_OBJ) vc4.qinc


# ------------------------------------------------
$(vc4asm_OBJ_DIR)/%.o: $(vc4asm_SRC_DIR)/%.cpp
	@mkdir -p $(vc4asm_OBJ_DIR)
	$(CXX) -c $< -o $@ $(vc4asm_CXX_FLAGS)

# ------------------------------------------------


install: build sdk_headers
	@mkdir -p "$(INSTALL_DIR)"
	@install -m 755 "$(OUT)" "$(INSTALL_DIR)/$(OUT)"

ifeq ($(INSTALL_FOR_EVERYONE),1)
	@ln -s "$(INSTALL_DIR)/$(OUT)" /bin/d3c                 2>/dev/null || true
else
	@ln -s "$(INSTALL_DIR)/$(OUT)" $(HOME)/.local/bin/d3c   2>/dev/null || true
endif

	@cp -r template/ $(INSTALL_DIR)/template

	@printf "\n"
	@printf "=============================================================\n"
	@printf "\033[32mSUCCESS\033[0m: $(OUT) installed to $(INSTALL_DIR)/$(OUT)\n"
	@printf "Make sure $(INSTALL_DIR) is in your PATH!\n"
	@printf "Put this command in your bashrc: \033[35mexport PATH=\"\$$PATH:$(INSTALL_DIR)\"\033[0m\n"
	@printf "   => Get started with: \033[35md3c init my_project\033[0m\n"
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


.PHONY: all clean fclean re install remove build vc4asm sdk_build sdk_headers

-include $(D_FILES)
