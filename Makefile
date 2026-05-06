CC				= aarch64-linux-gnu-gcc
LD				= aarch64-linux-gnu-ld
OBJ_COPY		= aarch64-linux-gnu-objcopy
QEMU			= qemu-system-aarch64
DEBUG			= 0
DEBUG_OUTLINE	= 0
LD_FLAGS		= -T linker.ld

SRC_DIR			= src
OBJ_DIR			= obj

C_FILES			= $(shell find $(SRC_DIR) -name "*.c")
S_FILES			= $(shell find $(SRC_DIR) -name "*.s" -o -name "*.S")
O_FILES			= $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_FILES))
O_FILES			+= $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(S_FILES))
D_FILES			= $(O_FILES:.o=.d)
DISK			?= disk.img

VERSION			= 2.0.3
VERSION_NAME	= outset
NAME			= d3m0n_os_$(VERSION_NAME)_$(VERSION).img
C_FLAGS			=	-Wall -Wextra -Werror -nostdlib -ffreestanding -O2 -Iincludes -mstrict-align \
					-MMD -MD \
					-D DEBUG=$(DEBUG) \
					-D DEBUG_OUTLINE=$(DEBUG_OUTLINE) \
					-D KERNEL_VERSION=\"$(VERSION)\" \
					-D KERNEL_VERSION_NAME=\"$(VERSION_NAME)\"



C1=\033[0;38;5;69;49m
C2=\033[0;38;5;105;49m
C3=\033[0;38;5;141;49m
C4=\033[0;38;5;177;49m
C5=\033[0;38;5;213;49m
COLOR_ERROR=\033[31m
COLOR_SUCCESS=\033[32m
R=\033[0m

all: banner $(NAME)

banner:
	@echo "    $(C1)██████╗ ██████╗ $(C1)███╗   ███╗ $(C1)██████╗ $(C2)███╗   ██╗$(R)"
	@echo "    $(C1)██╔══██╗╚════██╗$(C1)████╗ ████║$(C2)██╔═████╗$(C3)████╗  ██║$(R)"
	@echo "    $(C2)██║  ██║ █████╔╝$(C2)██╔████╔██║$(C3)██║██╔██║$(C4)██╔██╗ ██║$(R)"
	@echo "    $(C2)██║  ██║ ╚═══██╗$(C3)██║╚██╔╝██║$(C3)████╔╝██║$(C4)██║╚██╗██║$(R)"
	@echo "    $(C3)██████╔╝██████╔╝$(C4)██║ ╚═╝ ██║$(C4)╚██████╔╝$(C5)██║ ╚████║$(R)"
	@echo "    $(C5)╚═════╝ ╚═════╝ $(C5)╚═╝     ╚═╝ $(C5)╚═════╝ $(C5)╚═╝  ╚═══╝$(R)"
	@echo "                  $(C1)made by 4re5 group$(R)              "
	@echo "           $(C2)the first hacking cellular phone$(R)       \n\n"

$(NAME): $(O_FILES)
	@mkdir -p $(OBJ_DIR)
	@echo "Linking C objects: $^"
	@$(CC) $(C_FLAGS) $^ $(LD_FLAGS) -o $(OBJ_DIR)/kernel.elf
	@$(OBJ_COPY) -O binary $(OBJ_DIR)/kernel.elf $@
	@echo "$(COLOR_SUCCESS)Compilation done! => $(NAME)$(R)"

# C files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C object: $(C2)$<$(R)"
	@$(CC) $(C_FLAGS) -c $< -o $@

# S files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	@echo "Compiling C object: $(C2)$<$(R)"
	@$(CC) $(C_FLAGS) -c $< -o $@

disk:
	@dd if=/dev/zero of=$(DISK) bs=1M count=128
	@mkfs.vfat -F 32 -I -n D3M0NFS $(DISK)
	@mcopy -i $(DISK) -s rootfs/* ::
	@echo "$(COLOR_SUCCESS)Disk formatted as FAT32: $(DISK)$(R)"


run: all
	@test -f "$(DISK)" || (echo "$(COLOR_ERROR)Disk image not found: $(DISK)$(R)" && exit 1)
	@$(QEMU) \
		-machine raspi3ap \
		-cpu cortex-a53 \
		-serial stdio \
		-m 512M \
		-sd $(DISK) \
		-kernel obj/kernel.elf

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME) $(DISK)

re: fclean all

-include $(D_FILES)

.PHONY: all banner disk run clean fclean re