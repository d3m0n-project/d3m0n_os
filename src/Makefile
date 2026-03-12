CC				= aarch64-linux-gnu-gcc
LD				= aarch64-linux-gnu-ld
OBJ_COPY		= aarch64-linux-gnu-objcopy
QEMU			= qemu-system-aarch64
DEBUG			= 0
LD_FLAGS		= -T linker.ld
C_FILES			=	main.c \
					logging/uart.c \
					logging/log.c
S_FILES			= boot.s
OBJ_DIR			= obj
O_FILES			=	$(addprefix $(OBJ_DIR)/,$(notdir $(C_FILES:.c=.o))) \
					$(addprefix $(OBJ_DIR)/,$(notdir $(S_FILES:.s=.o)))
VERSION			= 2.0.1
VERSION_NAME	= outset
NAME			= d3m0n_os_$(VERSION_NAME)_$(VERSION).img
C_FLAGS			=	-Wall -Wextra -Werror -nostdlib -ffreestanding -Iincludes \
					-D DEBUG=$(DEBUG) \
					-D KERNEL_VERSION=\"$(VERSION)\" \
					-D KERNEL_VERSION_NAME=\"$(VERSION_NAME)\"


C1=\033[0;38;5;69;49m
C2=\033[0;38;5;105;49m
C3=\033[0;38;5;141;49m
C4=\033[0;38;5;177;49m
C5=\033[0;38;5;213;49m
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
	$(CC) $(C_FLAGS) $^ $(LD_FLAGS) -o $(OBJ_DIR)/kernel.elf
	$(OBJ_COPY) -O binary $(OBJ_DIR)/kernel.elf $@
	@echo "$(C2)Compilation done! => $(NAME)$(R)"

$(OBJ_DIR)/%.o: */%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.s
	@mkdir -p $(OBJ_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@

run: all
ifeq ($(DEBUG),1)
	$(QEMU) \
		-machine virt \
		-cpu cortex-a53 \
		-smp 4 \
		-m 512M \
		-nographic \
		-drive file=disk.img,format=raw,if=virtio \
		-kernel obj/kernel.elf
endif

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all banner run clean fclean re