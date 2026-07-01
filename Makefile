CC					= arm-none-eabi-gcc
LD					= arm-none-eabi-ld
OBJCOPY				= arm-none-eabi-objcopy

QEMU				= qemu-system-arm

# default config
DEBUG				= 0
DEBUG_OUTLINE		= 0
USE_ADMIN			= 1
LOGFILE_ENABLED		= 1
SHOW_IMAGE_STATUS	= 0
TRACK_MEMORY_USAGE	= 0

ifeq ($(USE_ADMIN), 1)
	SUDO_EXECUTABLE	= sudo
endif


SRC_DIR				= src
OBJ_DIR				= obj
BUILD_DIR			= build
EXPORT_DIR			= export
BOOT_DIR			= $(BUILD_DIR)/boot
MOUNT_DIR			= $(BUILD_DIR)/mnt
FIRMWARE_DIR		= firmware

C_FILES				= $(shell find $(SRC_DIR) -name "*.c")
S_FILES				= $(shell find $(SRC_DIR) -name "*.s" -o -name "*.S")

O_FILES				= $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_FILES))
O_FILES				+= $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(S_FILES))

D_FILES				= $(O_FILES:.o=.d)

DISK				?= disk.img

VERSION				= 2.0.6
VERSION_NAME		= outset

IMG_ROOT_NAME		= d3m0n_os_$(VERSION_NAME)_$(VERSION).img

NAME				= $(BUILD_DIR)/$(IMG_ROOT_NAME)
EXPORT_IMG_NAME 	= $(EXPORT_DIR)/$(IMG_ROOT_NAME)
ELF					= $(OBJ_DIR)/kernel.elf

IMG_SIZE			= 128

LD_FLAGS			= -T linker.ld -lgcc -Wl,-e,_start
C_FLAGS				= -Wall -Wextra -Werror -ffreestanding -nostdlib -O2 -g3 \
					  -Iincludes -mcpu=arm1176jzf-s \
					  -MMD -MD \
					  -D DEBUG=$(DEBUG) \
					  -D DEBUG_OUTLINE=$(DEBUG_OUTLINE) \
					  -D KERNEL_VERSION=\"$(VERSION)\" \
					  -D KERNEL_VERSION_NAME=\"$(VERSION_NAME)\" \
					  -D LOGFILE_ENABLED=$(LOGFILE_ENABLED) \
					  -D SHOW_IMAGE_STATUS=$(SHOW_IMAGE_STATUS) \
					  -D TRACK_MEMORY_USAGE=$(TRACK_MEMORY_USAGE)


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


all: banner show-config config $(NAME)


define onoff
$(if $(filter 1,$(1)),$(COLOR_SUCCESS)ENABLED$(R),$(COLOR_ERROR)DISABLED$(R))
endef

VARS := DEBUG DEBUG_OUTLINE USE_ADMIN LOGFILE_ENABLED SHOW_IMAGE_STATUS TRACK_MEMORY_USAGE
MSG_OUTLINE_COLOR=$(C1)
MSG_COLOR=$(C3)
show-config:
	@echo "    $(MSG_OUTLINE_COLOR)╔═════════════════╣ CONFIG ╠══════╦══════════╗$(R)"
	@$(foreach v,$(VARS), \
		printf "    $(MSG_OUTLINE_COLOR)║$(R) $(MSG_COLOR)%-31s$(R) $(MSG_OUTLINE_COLOR)║$(R) %-17b $(MSG_OUTLINE_COLOR)║$(R)\n" \
			"$(v)" \
			"$(call onoff,$($(v)))"; \
	)
	@echo "    $(MSG_OUTLINE_COLOR)╚═════════════════════════════════╩══════════╝$(R)"
	@echo ""

banner:
	@echo "    $(C1)██████╗ ██████╗ $(C1)███╗   ███╗ $(C1)██████╗ $(C2)███╗   ██╗$(R)"
	@echo "    $(C1)██╔══██╗╚════██╗$(C1)████╗ ████║$(C2)██╔═████╗$(C3)████╗  ██║$(R)"
	@echo "    $(C2)██║  ██║ █████╔╝$(C2)██╔████╔██║$(C3)██║██╔██║$(C4)██╔██╗ ██║$(R)"
	@echo "    $(C2)██║  ██║ ╚═══██╗$(C3)██║╚██╔╝██║$(C3)████╔╝██║$(C4)██║╚██╗██║$(R)"
	@echo "    $(C3)██████╔╝██████╔╝$(C4)██║ ╚═╝ ██║$(C4)╚██████╔╝$(C5)██║ ╚████║$(R)"
	@echo "    $(C5)╚═════╝ ╚═════╝ $(C5)╚═╝     ╚═╝ $(C5)╚═════╝ $(C5)╚═╝  ╚═══╝$(R)"
	@echo ""


CONFIG_FILE = $(OBJ_DIR)/.config
CONFIG_HASH = $(shell printf '%s\n' $(foreach v,$(VARS),$(v)=$($(v))) | sha1sum | awk '{print $$1}')

$(CONFIG_FILE):
	@true

config:
	@mkdir -p $(OBJ_DIR)
	@printf '%s\n' $(foreach v,$(VARS),$(v)=$($(v))) | sha1sum | awk '{print $$1}' > $(CONFIG_FILE).tmp
	@if ! cmp -s $(CONFIG_FILE).tmp $(CONFIG_FILE) 2>/dev/null; then \
		mv $(CONFIG_FILE).tmp $(CONFIG_FILE); \
		echo "Config changed, recompiling..."; \
	else \
		rm -f $(CONFIG_FILE).tmp; \
	fi

$(O_FILES): $(CONFIG_FILE)

$(NAME): $(O_FILES)
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)

	@echo "$(COLOR_INFO)[LD] Linking kernel...$(R)"
	$(CC) $(C_FLAGS) $(O_FILES) $(LD_FLAGS) -o $(OBJ_DIR)/kernel.elf

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


applications:
	@rm -rf rootfs/apps/ > /dev/null
	@mkdir -p rootfs/apps/
	@mkdir -p rootfs/apps/d3m0n/
	@cp -r applications/* rootfs/apps/d3m0n/
	@echo "$(COLOR_SUCCESS)[OK] Copied applications list to disk!$(R)"


disk: applications
	@echo "$(COLOR_INFO)[IMG] Creating disk image...$(R)"
	@dd if=/dev/zero of=$(DISK) bs=1M count=$(IMG_SIZE)
	@$(SUDO_EXECUTABLE) parted $(DISK) mklabel msdos
	@$(SUDO_EXECUTABLE) parted $(DISK) mkpart primary fat32 1MiB 100%
	@mkfs.vfat -F 32 -n D3M0NFS $(DISK)
	@mcopy -i $(DISK) -s rootfs/* ::
	@rm -rf rootfs/apps/ > /dev/null
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

	@LOOP=$$($(SUDO_EXECUTABLE) losetup --find --show --partscan $(EXPORT_IMG_NAME)); \
	$(SUDO_EXECUTABLE) partprobe $$LOOP || true; \
	sleep 1; \
	$(SUDO_EXECUTABLE) mkfs.vfat -F 32 $${LOOP}p1; \
	$(SUDO_EXECUTABLE) mkfs.vfat -F 32 $${LOOP}p2; \
	mkdir -p $(MOUNT_DIR)/boot; \
	mkdir -p $(MOUNT_DIR)/root; \
	$(SUDO_EXECUTABLE) mount $${LOOP}p1 $(MOUNT_DIR)/boot; \
	$(SUDO_EXECUTABLE) mount $${LOOP}p2 $(MOUNT_DIR)/root; \
	$(SUDO_EXECUTABLE) cp $(BUILD_DIR)/kernel.img $(MOUNT_DIR)/boot/kernel.img; \
	$(SUDO_EXECUTABLE) cp -r $(FIRMWARE_DIR)/* $(MOUNT_DIR)/boot/ 2>/dev/null || true; \
	$(SUDO_EXECUTABLE) cp -r rootfs/* $(MOUNT_DIR)/root/; \
	sync; \
	$(SUDO_EXECUTABLE) umount $(MOUNT_DIR)/boot; \
	$(SUDO_EXECUTABLE) umount $(MOUNT_DIR)/root; \
	$(SUDO_EXECUTABLE) losetup -d $$LOOP

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
		-kernel $(OBJ_DIR)/kernel.elf \
		-usb -device usb-mouse


clean:
	@echo "$(COLOR_WARN)[CLEAN] Removing objects...$(R)"
	@rm -rf $(OBJ_DIR) $(BUILD_DIR)

fclean: clean
	@echo "$(COLOR_WARN)[FCLEAN] Removing exports...$(R)"
	@rm -rf $(EXPORT_DIR) $(DISK)

re: fclean all

-include $(D_FILES)

.PHONY: all banner disk run clean fclean re export show-config applications config