
# ![icon](https://avatars.githubusercontent.com/u/136185636?s=40&u=76e3b4569d8be273bb0358ae3c71fcdf7c7b2d6b&v=4) d3m0n os (C) 2023 4re5 group

d3m0n OS is a Debian based ARM CPU architecture operating system. 

> :warning: This code is made to run on a d3m0n device only :warning:

d3m0n app system and binaries are located in [rootfs/usr/share/d3m0n](https://github.com/d3m0n-project/d3m0n_os/tree/main/rootfs/usr/share/d3m0n)


## Summary

(1) [Installation](https://github.com/d3m0n-project/d3m0n_os#Installation)

(2) [Configuration](https://github.com/d3m0n-project/d3m0n_os#Configure_your_OS)

(3) [Application API](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/api.md)

(4) [Installation](https://github.com/d3m0n-project/d3m0n_os#Installation)









## Partitions

> /[boot](https://github.com/d3m0n-project/d3m0n_os/tree/main/boot)		is a ext4 bootloader partition that contains the linux kernel and will make all this operating system work. It'll be located at /boot in [rootfs](https://github.com/d3m0n-project/d3m0n_os/tree/main/rootfs).
> 
> /[rootfs](https://github.com/d3m0n-project/d3m0n_os/tree/main/rootfs)	is the root partition, it contains call linux commands that d3m0n os needs to work.

## Installation

To execute this code and build your d3m0n os image (.img), you must execute these commands **line by line** in your linux machine terminal.

    sudo apt-get install git
    git clone https://github.com/d3m0n-project/d3m0n_os.git
    cd d3m0n_os
    sudo chmod +x *.sh && sudo chmod +x config
    sudo ./d3m0n.sh
then use numbers to select option you wan't

## Configure your OS

In order to build your image file (.img) you need first to install [d3m0n os](ps://github.com/d3m0n-project/d3m0n_os#installation) on your linux machine.
Then use the command `sudo ./d3m0n.sh` and type **2** and hit enter.
This will display a configuration window in your terminal.

## Flash a SD card with a image file

Install **BalenaEtcher** from [here](https://etcher.balena.io/#download-etcher) and select your image file (.img) 
![add_img](https://github.com/d3m0n-project/d3m0n_os/assets/71982379/fec65775-27f7-4106-bdbc-fe4f6d366b1d)
Select your SD card drive
![image](https://github.com/d3m0n-project/d3m0n_os/assets/71982379/239616df-6634-48e5-8bb4-a3b139993d13)
 And click **FLASH**

## Delete a file

You can delete the current file by clicking the **Remove** button in the file explorer. The file will be moved into the **Trash** folder and automatically deleted after 7 days of inactivity.

## Export a file

You can export the current file by clicking **Export to disk** in the menu. You can choose to export the file as plain Markdown, as HTML using a Handlebars template or as a PDF.


# d3m0n api

d3m0n operating system contains preinstalled softwares.
- d3m0n apps
  - communication
    - contacts
    - phone
    - messages   
  - system
    - settings  
  - photo
    - camera
    - gallery 
  - misc
    - files 
    - calculator
    - browser
    - store
- keyboard
  - keyboard app
- system apps
   - touchscreen (touch screen handle software)
   - lockscreen

## wallpapers
Wallpapers are located in `/usr/share/d3m0n/wallpapers`. To change it, edit `wallpaper` in d3m0n [config file](#config-file)
![image](https://github.com/d3m0n-project/d3m0n_os/assets/71982379/5aa3aa21-c8a5-446c-9387-479a81cb5e0a)

## config file
	wallpaper: default
	theme: default_dark
	splash_time: 5000
### description
`wallpaper` is your wallpaper name (without .png)
`theme` is your theme folder name that contains all icons and sounds
`splash_time` is the time in ms splash screen will be displayed
