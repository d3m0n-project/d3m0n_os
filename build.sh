#!/bin/bash 

# apps are in             usr\share\raspi-ui-overrides\applications
# wallpapers are in       usr\share\rpd-wallpaper





IMG_NAME="d3m0n_os.img"

FIRST_USER_NAME="d3m0n"

BUILD_DIR="build"
ROOTFS_DIR="${BUILD_DIR}/rootfs"
BOOTFS_DIR="${BUILD_DIR}/boot"

echo -e "\e[33m[~] Loading config\e[0m"
echo -e "IMG_NAME:            ${IMG_NAME}"
echo -e "FIRST_USER_NAME:     ${FIRST_USER_NAME}"
echo -e "BUILD_DIR:           ${BUILD_DIR}"
echo -e "ROOTFS_DIR:          ${ROOTFS_DIR}"
echo -e "BOOTFS_DIR:          ${BOOTFS_DIR}"

sleep 2


echo -e "\e[33m[~] Clearing /dev/loop0\e[0m"
sudo losetup -d /dev/loop0  > /dev/null
echo -e "\e[33m[~] Deleting last builds\e[0m"
rm -rf $BUILD_DIR > /dev/null
rm -rf $IMG_NAME > /dev/null


echo -e "\e[33m[~] Creating empty img file\e[0m"
dd if=/dev/zero of=$IMG_NAME bs=1M count=8000

echo -e "\e[33m[~] Attaching it to the device\e[0m"
sudo losetup -f --show $IMG_NAME
# sudo losetup --show -f -o ${BOOT_OFFSET} --sizelimit ${BOOT_LENGTH} ${IMG_NAME}

echo -e "\e[33m[~] Calculating partitions sizes\e[0m"

echo -e "\e[33m[~] Formating the partition tables\e[0m"
sudo parted /dev/loop0 --script mktable msdos 
sudo parted /dev/loop0 --script mkpart primary fat32 1MiB 300MiB
sudo parted /dev/loop0 --script mkpart primary ext4 300MiB 100%

echo -e "\e[33m[~] Formating partitions file systems\e[0m"
sudo mkfs.vfat /dev/loop0p1
sudo mkfs.ext4 /dev/loop0p2



# FAT32 > bootloader/kernel partition
# ext4  > root filesystem partition



echo -e "\e[33m[~] Creating building directories\e[0m"
mkdir $BUILD_DIR
mkdir $BOOTFS_DIR
mkdir $ROOTFS_DIR

echo -e "\e[33m[~] Adding bootloader/kernel and rootfs folders path to img file\e[0m"
sudo mount /dev/loop0p1 $BOOTFS_DIR
sudo mount /dev/loop0p2 $ROOTFS_DIR

# copy files to 
echo -e "\e[33m[~] Copying files to folders path to img file (can take a while)\e[0m"
cp -r boot/* $BOOTFS_DIR
echo -e "     > [o] bootloader/kernel"
cp -r rootfs/* $ROOTFS_DIR
echo -e "     > [o] rootfs"

echo -e "done!"


# system edit part
echo -e "\e[33m[~] Fetching root&boot partitons uuids\e[0m"
BOOT_PARTUUID=$(blkid -o value -s PARTUUID /dev/loop0p1)
ROOT_PARTUUID=$(blkid -o value -s PARTUUID /dev/loop0p2)

echo -e "proc            /proc           proc    defaults          0       0
PARTUUID=${BOOT_PARTUUID}  /boot           vfat    defaults          0       2
PARTUUID=${ROOT_PARTUUID}  /               ext4    defaults,noatime  0       1" > "${ROOTFS_DIR}/etc/fstab"
cat "${ROOTFS_DIR}/etc/fstab"


# echo -e "console=serial0,115200 console=tty1 root=PARTUUID=$(blkid -o value -s PARTUUID /dev/loop0p2) rootfstype=ext4 fsck.repair=yes rootwait quiet init=/usr/lib/raspi-config/init_resize.sh splash plymouth.ignore-serial-consoles systemd.run=/boot/firstrun.sh systemd.run_success_action=reboot systemd.unit=kernel-command-line.target" > $BOOTFS_DIR/cmdline.txt
echo -e "console=serial0,115200 console=tty1 root=PARTUUID=${ROOT_PARTUUID} rootfstype=ext4 fsck.repair=yes rootwait quiet init=/usr/lib/raspberrypi-sys-mods/firstboot splash plymouth.ignore-serial-consoles systemd.run=/boot/firstrun.sh systemd.run_success_action=reboot systemd.unit=kernel-command-line.target" > "${BOOTFS_DIR}/cmdline.txt"
cat "${BOOTFS_DIR}/cmdline.txt"


echo -e "\e[33m[~] Setting system chroot & chmod permissions\e[0m"

bash ./perms.sh

echo -e "\e[33m[~] Saving finnal img file\e[0m"
sudo umount $ROOTFS_DIR
sudo umount $BOOTFS_DIR

echo -e "\e[33m[~] Detaching img from host deive\e[0m"
sudo losetup -d /dev/loop0

echo -e "\e[33m[~] Cleaning...\e[0m"
rm -rf build
echo -e "\e[32m[o] Successfully build your ${IMG_NAME} image\e[0m"
echo -e "\e[32mEnjoy ;)\e[0m"