# ![icon](https://avatars.githubusercontent.com/u/136185636?s=40&u=76e3b4569d8be273bb0358ae3c71fcdf7c7b2d6b&v=4) d3m0n os (C) 2024 4re5 group


<p align="center">
    <a href="https://github.com/d3m0n-project/d3m0n_os/releases/latest"><img alt="Release" src="https://img.shields.io/github/release/d3m0n-project/d3m0n_os.svg?style=flat-square"></a>
    <a href="https://github.com/d3m0n-project/d3m0n_os/blob/master/LICENSE.md"><img alt="Software License" src="https://img.shields.io/badge/license-GPL3-brightgreen.svg?style=flat-square"></a>
    <a href="https://github.com/d3m0n-project/d3m0n_os/graphs/contributors"><img alt="Contributors" src="https://img.shields.io/github/contributors/d3m0n-project/d3m0n_os"/></a>
    <a href="https://twitter.com/intent/follow?screen_name=4re5group"><img src="https://img.shields.io/twitter/follow/4re5group?style=social&logo=twitter" alt="follow on Twitter"></a>
</p>

d3m0n OS is a Debian based ARM CPU architecture operating system. 
> :warning: This code is made to run on a d3m0n device only or only on linux distibutions for debug purposes :warning:

> We had to remake the whole system from scratch in C instead of C#
d3m0n app system and binaries are located in [rootfs/usr/share/d3m0n](https://github.com/d3m0n-project/d3m0n_os/tree/main/rootfs/usr/share/d3m0n)

![d3m0n_ssh](https://github.com/d3m0n-project/d3m0n_os/assets/71982379/c989aebe-c06f-4417-821c-ac0029023d80)



> ## Summary
>
>   (1) [Installation](https://github.com/d3m0n-project/d3m0n_os#Installation)
>
>   (2) [Configuration](https://github.com/d3m0n-project/d3m0n_os#Configure_your_OS)
>
>   (3) [Application API](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/api.md)
>
>   (4) [About](https://github.com/d3m0n-project/d3m0n_os#About)
>
>   (5) [Links](https://github.com/d3m0n-project/d3m0n_os#Links)





## Partitions

> /[boot](https://github.com/d3m0n-project/d3m0n_os/tree/main/boot)		is a ext4 bootloader partition that contains the linux kernel and will make all this operating system work. It'll be located at /boot in [rootfs](https://github.com/d3m0n-project/d3m0n_os/tree/main/rootfs).
> 
> /[rootfs](https://github.com/d3m0n-project/d3m0n_os/tree/main/rootfs)	is the root partition, it contains call linux commands that d3m0n os needs to work.
## Installations
### Flash pre-built image
Just download lastest [release](https://github.com/d3m0n-project/d3m0n_os/releases) and flash it using Balena Etcher
<details>
  <summary>How?</summary>
  <p>- Install <b>BalenaEtcher</b> from <a href="https://etcher.balena.io/#download-etcher">here</a> and select your image file (.img) </p>
  <img style="width: 400px;" src="https://github.com/d3m0n-project/d3m0n_os/assets/71982379/fec65775-27f7-4106-bdbc-fe4f6d366b1d">
  <p>- Select your SD card drive</p>
  <img style="width: 400px;" src="https://github.com/d3m0n-project/d3m0n_os/assets/71982379/239616df-6634-48e5-8bb4-a3b139993d13">
  <p>- And click <b>FLASH</b></p>
</details>

### Install d3m0n system on an already flashed **raspberry pi zero 2W**
First step, install wget using `sudo apt-get install wget -y`

    mkdir d3m0n-install
    cd d3m0n-install
    wget https://raw.githubusercontent.com/d3m0n-project/d3m0n_os/root/install.sh
    sudo chmod +x install.sh
    sudo ./install.sh`
Finaly wait until installation complete 


### Build custom image
To execute this code and build your d3m0n os image (.img), you must execute these commands **line by line** in your linux machine terminal.

    sudo apt-get install git
    git clone https://github.com/d3m0n-project/d3m0n_os.git
    cd d3m0n_os
    sudo chmod +x *.sh && sudo chmod +x config
    sudo ./d3m0n.sh
then use numbers to select option you wan't

## Configure your OS

In order to build your image file (.img) you need first to install [d3m0n os](ps://github.com/d3m0n-project/d3m0n_os#installations) on your linux machine.
Then use the command `sudo ./d3m0n.sh` and type **2** and hit enter.
This will display a configuration window in your terminal.


# d3m0n api

d3m0n operating system contains preinstalled softwares.
[more information here](./rootfs/usr/share/applications)

[example .d3m0n app](https://github.com/d3m0n-project/d3m0n_os/tree/main/rootfs/usr/share/d3m0n/apps/test_app/source)
## App handler
Phone loads apps from temp directory created when startup, 

then reads manifest file located at `/usr/share/d3m0n/temp/APP_TOKEN/app`
- Gets app name, description, category, icon (format "key: value")
then reads main layout file (.layout) in `/usr/share/d3m0n/temp/APP_TOKEN/layouts/main.layout` (example [here](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/apps/test_app/source/layouts/main.layout))

then execute code (.src) in `/usr/share/d3m0n/temp/APP_TOKEN/src/main.src` (example [here](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/apps/test_app/source/src/main.src))


## wallpapers
Wallpapers are located in `/usr/share/d3m0n/wallpapers`. To change it, edit `wallpaper` in d3m0n [config file](#config-file)
![image](https://github.com/d3m0n-project/d3m0n_os/assets/71982379/5aa3aa21-c8a5-446c-9387-479a81cb5e0a)

## config file
	wallpaper: default
	theme: default_dark
	splash_time: 3000
	shell_port: 3849
### description
`wallpaper` is your wallpaper name (without .png)

`theme` is your theme folder name that contains all icons and sounds

`splash_time` is the time in ms splash screen will be displayed

`shell_port` is machine port where d3m0n shell service will run


## Links

&nbsp; | Official Links
---------|-------
Website | [d3m0n-project.github.io](https://d3m0n-project.github.io/d3m0n_os/)
Forum | [forum.ducksploit.com](https://forum.ducksploit.com/)
Discord | [discord.com](https://discord.com/invite/TQTEvt2wsb)
Subreddit | [r/d3m0nproject](https://www.reddit.com/r/d3m0nproject/)

## Made with ❤️ by 4RE5's devs
<br>
<a href="https://github.com/d3m0n-project/d3m0n_os/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=d3m0n-project/d3m0n_os" />
</a>
