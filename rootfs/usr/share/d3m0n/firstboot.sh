#!/bin/bash

# update
sudo apt-get update -y

# installing libs
# sudo apt-get install mono-complete -y
# sudo apt install xorg xserver-xorg xinit cmake git -y 
sudo apt install cmake git -y 
# sudo apt-get install --reinstall libgtk2.0-0 -y
sudo apt-get install paplay -y
# sudo apt-get install make libsndfile1-dev -y
sudo apt install libopencv-dev -y


# setting correct perms
sudo chmod +rx /usr/share/d3m0n
sudo chmod +rx /usr/share/d3m0n/*
sudo chmod +rx /usr/share/d3m0n/display/*
sudo chmod +rx /usr/share/d3m0n/apps/*
sudo chmod +rx /usr/share/d3m0n/ssh
sudo chmod +rx /usr/share/d3m0n/ssh/*



# creating display driver
# sudo mkdir /usr/share/d3m0n/display/source
# cd /usr/share/d3m0n/display/source
# sudo git clone https://github.com/juj/fbcp-ili9341.git
# cd fbcp-ili9341
# sudo mkdir build
# cd build
# sudo cmake ../ -DILI9341=ON -DDISPLAY_ROTATE_180_DEGREES=ON -DGPIO_TFT_DATA_CONTROL=24 -DGPIO_TFT_RESET_PIN=25 -DSPI_BUS_CLOCK_DIVISOR=6 -DSTATISTICS=0
# sudo make -j
# sudo cp ./fbcp-ili9341 /usr/share/d3m0n/display/fbcp-ili9341


# downloading bcm2835
# sudo mkdir /usr/share/d3m0n/temp/
# cd /usr/share/d3m0n/temp/
# wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.56.tar.gz
# tar zxvf bcm2835-1.56.tar.gz
# cd bcm2835-1.56
# ./configure
# make
# sudo make check
# sudo make install




cd ~

# formating boot config file
sudo rm -rf /boot/config.txt

sudo echo "# For more options and information see
# http://rpf.io/configtxt
# Some settings may impact device functionality. See link above for details

# uncomment if you get no picture on HDMI for a default safe mode
#hdmi_safe=1

# uncomment the following to adjust overscan. Use positive numbers if console
# goes off screen, and negative if there is too much border
#overscan_left=16
#overscan_right=16
#overscan_top=16
#overscan_bottom=16

# uncomment to force a console size. By default it will be display's size minus
# overscan.
#framebuffer_width=1280
#framebuffer_height=720

# uncomment if hdmi display is not detected and composite is being output
#hdmi_force_hotplug=1
display_lcd_rotate=1
display_hdmi_rotate=1
# uncomment to force a specific HDMI mode (this will force VGA)
#hdmi_group=2
#hdmi_mode=16

# uncomment to force a HDMI mode rather than DVI. This can make audio work in
# DMT (computer monitor) modes
#hdmi_drive=2

# uncomment to increase signal to HDMI, if you have interference, blanking, or
# no display
#config_hdmi_boost=4

# uncomment for composite PAL
#sdtv_mode=2

#uncomment to overclock the arm. 700 MHz is the default.
#arm_freq=800

# Uncomment some or all of these to enable the optional hardware interfaces
dtparam=i2c_arm=on
dtparam=i2s=on
dtparam=spi=on

# Uncomment this to enable infrared communication.
#dtoverlay=gpio-ir,gpio_pin=17
#dtoverlay=gpio-ir-tx,gpio_pin=18

# Additional overlays and parameters are documented /boot/overlays/README

# Enable audio (loads snd_bcm2835)
dtparam=audio=on

# Automatically load overlays for detected cameras
start_x=1

# Automatically load overlays for detected DSI displays
display_auto_detect=1

# Enable DRM VC4 V3D driver
#dtoverlay=vc4-kms-v3d
max_framebuffers=2

# Disable compensation for displays with overscan
disable_overscan=1

[cm4]
# Enable host mode on the 2711 built-in XHCI USB controller.
# This line should be removed if the legacy DWC2 controller is required
# (e.g. for USB device mode) or if USB support is not required.
otg_mode=1

[all]

[pi4]
dtoverlay=vc4-fkms-v3d
# Run as fast as firmware / board allows
arm_boost=1

[all]
dtoverlay=spi0-1cs,cs0_pin=CE0,cs0_spidev=disabled,speed=64000000
#dtoverlay=waveshare35a,fps=30,speed=42000000
#framebuffer_width=240
#framebuffer_height=320
#hdmi_pixel_encoding=2
#hdmi_cvt=240 320 60 2 1
#hdmi_group=2
#hdmi_mode=16
#hdmi_drive=2




# power optimisation
dtparam=act_led_trigger=none
dtparam=act_led_activelow=on
dtoverlay=disable-bt

# enable serial communication
enable_uart=1
dtoverlay=pi3-miniuart-bt

# boost performances
force_turbo=1
gpu_mem=128" > /boot/config.txt

# also delete this console=serial0, 115200  in /boot/cmdline.txt





sudo rm -rf /etc/rc.local

echo "#!/bin/sh -e
#
# rc.local
#
# This script is executed at the end of each multiuser runlevel.
# Make sure that the script will "exit 0" on success or any other
# value on error.
#
# In order to enable or disable this script just change the execution
# bits.
#
# By default this script does nothing.

sudo bash /usr/share/d3m0n/startup.sh &

exit 0" > /etc/rc.local

sudo chmod +x /etc/rc.local


echo "Banner /usr/share/d3m0n/ssh/banner" >> /etc/ssh/sshd_config



# enable autologin
cat > /etc/systemd/system/getty@tty1.service.d/autologin.conf << EOF
[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin $USER --noclear %I \$TERM
EOF


# setting up drivers
sudo dpkg -i /usr/share/d3m0n/lib/nrf24l01.deb

sudo reboot now

exit 0