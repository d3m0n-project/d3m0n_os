CC				= arm-none-eabi-gcc
LD				= arm-none-eabi-ld
OBJCOPY			= arm-none-eabi-objcopy

QEMU			= qemu-system-arm

DEBUG			= 0
DEBUG_OUTLINE	= 0

SRC_DIR			= src
OBJ_DIR			= obj
BUILD_DIR		= build
EXPORT_DIR		= export
BOOT_DIR		= $(BUILD_DIR)/boot
MOUNT_DIR		= $(BUILD_DIR)/mnt
FIRMWARE_DIR	= firmware

C_FILES			= $(shell find $(SRC_DIR) -name "*.c")
S_FILES			= $(shell find $(SRC_DIR) -name "*.s" -o -name "*.S")

O_FILES			= $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_FILES))
O_FILES			+= $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(S_FILES))

D_FILES			= $(O_FILES:.o=.d)

DISK			?= disk.img

VERSION			= 2.0.5
VERSION_NAME	= outset

IMG_ROOT_NAME	= d3m0n_os_$(VERSION_NAME)_$(VERSION).img

NAME			= $(BUILD_DIR)/$(IMG_ROOT_NAME)
EXPORT_IMG_NAME = $(EXPORT_DIR)/$(IMG_ROOT_NAME)
ELF				= $(OBJ_DIR)/kernel.elf

IMG_SIZE		= 128

LD_FLAGS		= -T linker.ld -lgcc -Wl,-e,_start
C_FLAGS			= -Wall -Wextra -Werror -ffreestanding -nostdlib -O2 \
				  -Iincludes -mcpu=arm1176jzf-s \
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

COLOR_INFO=\033[36m
COLOR_ERROR=\033[31m
COLOR_SUCCESS=\033[32m
COLOR_WARN=\033[33m
R=\033[0m


all: banner $(NAME)

banner:
	@echo "    $(C1)██████╗ ██████╗ $(C1)███╗   ███╗ $(C1)██████╗ $(C2)███╗   ██╗$(R)"
	@echo "    $(C1)██╔══██╗╚════██╗$(C1)████╗ ████║$(C2)██╔═████╗$(C3)████╗  ██║$(R)"
	@echo "    $(C2)██║  ██║ █████╔╝$(C2)██╔████╔██║$(C3)██║██╔██║$(C4)██╔██╗ ██║$(R)"
	@echo "    $(C2)██║  ██║ ╚═══██╗$(C3)██║╚██╔╝██║$(C3)████╔╝██║$(C4)██║╚██╗██║$(R)"
	@echo "    $(C3)██████╔╝██████╔╝$(C4)██║ ╚═╝ ██║$(C4)╚██████╔╝$(C5)██║ ╚████║$(R)"
	@echo "    $(C5)╚═════╝ ╚═════╝ $(C5)╚═╝     ╚═╝ $(C5)╚═════╝ $(C5)╚═╝  ╚═══╝$(R)"


$(NAME): $(O_FILES)
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)

	@echo "$(COLOR_INFO)[LD] Linking kernel...$(R)"
#	$(CC) -ffreestanding -nostdlib -nostartfiles -T linker.ld -Wl,-e,_start -o $(OBJ_DIR)/kernel.elf $^ -lgcc
	$(CC) $(C_FLAGS) $^ $(LD_FLAGS) -o $(OBJ_DIR)/kernel.elf

	@echo "$(COLOR_INFO)[OBJCOPY] Creating kernel.img...$(R)"
	@$(OBJCOPY) -O binary $(OBJ_DIR)/kernel.elf $(BUILD_DIR)/kernel.img

	@cp $(BUILD_DIR)/kernel.img $(NAME)

	@echo "$(COLOR_SUCCESS)[OK] Build done: $(NAME)$(R)"


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "$(COLOR_INFO)[CC] $<$(R)"
	@$(CC) $(C_FLAGS) -c $< -o $@


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	@echo "$(COLOR_INFO)[AS] $<$(R)"
	@$(CC) $(C_FLAGS) -c $< -o $@


disk:
	@echo "$(COLOR_INFO)[IMG] Creating disk image...$(R)"
	@dd if=/dev/zero of=$(DISK) bs=1M count=$(IMG_SIZE)
	@sudo parted $(DISK) mklabel msdos
	@sudo parted $(DISK) mkpart primary fat32 1MiB 100%
	@mkfs.vfat -F 32 -n D3M0NFS $(DISK)
	@mcopy -i $(DISK) -s rootfs/* ::
	@echo "$(COLOR_SUCCESS)[OK] Disk ready: $(DISK)$(R)"




export: all
	@echo "$(COLOR_INFO)[IMG] Creating SD image...$(R)"

	@mkdir -p $(EXPORT_DIR)
	@mkdir -p $(BOOT_DIR)
	@mkdir -p $(MOUNT_DIR)

	@dd if=/dev/zero of=$(EXPORT_IMG_NAME) bs=1M count=$(IMG_SIZE)

	@parted $(EXPORT_IMG_NAME) --script mklabel msdos
	@parted $(EXPORT_IMG_NAME) --script mkpart primary fat32 1MiB 65MiB
	@parted $(EXPORT_IMG_NAME) --script mkpart primary fat32 65MiB 100%
	@parted $(EXPORT_IMG_NAME) --script set 1 boot on

	@sync

	@LOOP=$$(sudo losetup --find --show --partscan $(EXPORT_IMG_NAME)); \
	sudo partprobe $$LOOP || true; \
	sleep 1; \
	sudo mkfs.vfat -F 32 $${LOOP}p1; \
	sudo mkfs.vfat -F 32 $${LOOP}p2; \
	mkdir -p $(MOUNT_DIR)/boot; \
	mkdir -p $(MOUNT_DIR)/root; \
	sudo mount $${LOOP}p1 $(MOUNT_DIR)/boot; \
	sudo mount $${LOOP}p2 $(MOUNT_DIR)/root; \
	sudo cp $(BUILD_DIR)/kernel.img $(MOUNT_DIR)/boot/kernel.img; \
	sudo cp -r $(FIRMWARE_DIR)/* $(MOUNT_DIR)/boot/ 2>/dev/null || true; \
	sudo cp -r rootfs/* $(MOUNT_DIR)/root/; \
	sync; \
	sudo umount $(MOUNT_DIR)/boot; \
	sudo umount $(MOUNT_DIR)/root; \
	sudo losetup -d $$LOOP

	@echo "$(COLOR_SUCCESS)[OK] Bootable SD image created: $(EXPORT_IMG_NAME)$(R)"


run: all
	@echo "$(COLOR_WARN)[QEMU] Starting emulator...$(R)"
	@test -f "$(DISK)" || (echo "$(COLOR_ERROR)Disk image not found: $(DISK)$(R)" && exit 1)
	@$(QEMU) \
		-machine raspi1ap \
		-cpu arm1176 \
		-serial mon:stdio \
		-m 512M \
		-sd $(DISK) \
		-kernel $(OBJ_DIR)/kernel.elf


clean:
	@echo "$(COLOR_WARN)[CLEAN] Removing objects...$(R)"
	@rm -rf $(OBJ_DIR) $(BUILD_DIR)

fclean: clean
	@echo "$(COLOR_WARN)[FCLEAN] Removing exports...$(R)"
	@rm -rf $(EXPORT_DIR) $(DISK)

re: fclean all

-include $(D_FILES)

.PHONY: all banner disk run clean fclean re export