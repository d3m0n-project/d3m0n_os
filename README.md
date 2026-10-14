# ![icon](https://avatars.githubusercontent.com/u/136185636?s=40&u=76e3b4569d8be273bb0358ae3c71fcdf7c7b2d6b&v=4) d3m0n os (C) 2026 4re5 group


<p align="center">
    <a href="https://github.com/d3m0n-project/d3m0n_os/releases/latest"><img alt="Release" src="https://img.shields.io/github/release/d3m0n-project/d3m0n_os.svg?style=flat-square"></a>
	<img src="https://github.com/d3m0n-project/d3m0n_os/actions/workflows/compilation.yml/badge.svg">
  <img src="https://github.com/d3m0n-project/d3m0n_os/actions/workflows/doxygen.yml/badge.svg">
    <a href="https://github.com/d3m0n-project/d3m0n_os/blob/master/LICENSE.md"><img alt="Software License" src="https://img.shields.io/badge/license-GPL3-brightgreen.svg?style=flat-square"></a>
    <a href="https://github.com/d3m0n-project/d3m0n_os/graphs/contributors"><img alt="Contributors" src="https://img.shields.io/github/contributors/d3m0n-project/d3m0n_os"/></a>
    <a href="https://twitter.com/intent/follow?screen_name=4re5group"><img src="https://img.shields.io/twitter/follow/4re5group?style=social&logo=twitter" alt="follow on Twitter"></a>
</p>
d3m0n OS is the official firmware for the d3m0n devices.

> [!NOTE]
> We had to remake the whole system from scratch in C instead of C++, the project is still in beta testing

<!--![d3m0n_ssh](https://github.com/d3m0n-project/d3m0n_os/assets/71982379/c989aebe-c06f-4417-821c-ac0029023d80)-->



> ## Summary
>   1. [Project Overview](#overview)
>   2. [Installation](#Installation)<br>
>   3. [Configuration](Configure_your_OS)<br>
>   4. [SDK Documentation](https://d3m0n.4re5.com/d3m0n_os/)<br>
>   5. [About](#about)<br> 
>   6. [Links](#links)<br>



## Overview
This project aims to contribute and maintain the official `d3m0n c1` firmware. More info about the main project [here](https://github.com/d3m0n-project/d3m0n_c1).

> [!Note]



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

### Build kernel image
To execute this code and build your d3m0n os image (.img), you must execute these commands **line by line** in your linux machine terminal.
```bash
git clone --recurse-submodules https://github.com/d3m0n-project/d3m0n_os.git
cd d3m0n_os
make export
```


## Configure your OS

Multiple `Makefile variables` can be overrided in order to allow users to add debug informations, remove features or just change details of their exported firmware.
| Variable name         | Default Value | Description                                      | Usage example                            |
| --------------------- | ------------- | ------------------------------------------------ | ---------------------------------------- |
| DEBUG	                | 0             | If set, switch from real mode to simulation mode | `make run DEBUG=1`                       |
| DEBUG_OUTLINE       	| 0             | Draws the control outlines                       | `make run DEBUG=1 DEBUG_OUTLINE=1`       |
| USE_ADMIN	            | 1             | Use admin rights to compile or make disk         | `make disk USE_ADMIN=0`                  |
| SHOW_IMAGE_STATUS	    | 0             | Display the loaded images sizes, format...       | `make run DEBUG=1 SHOW_IMAGE_STATUS=1`   |
| TRACK_MEMORY_USAGE	  | 0             | Display the amount of allocated heap bytes       | `make run DEBUG=1 TRACK_MEMORY_USAGE=1`  |
| USE_GDB	              | 0             | Launch in debug mode with gdb server             | `make run DEBUG=1 USE_GDB=1`             |
| BUILD_APPS	          | 1             | Build and loads applications at disk creation    | `make run DEBUG=1 BUILD_APPS=0`          |


# d3m0n api

d3m0n operating system contains preinstalled softwares.
[more information here](./applications/)

[example .d3m0n app](./compiler/template/)

## Wallpapers
Wallpapers are located in `/rootfs/wallpapers`. To change it, edit `wallpaper` in d3m0n [config file](#config-file)


## Config file
A configuration file is provided at the disk root [/config](./rootfs/config).
It can be used to modify your system style and behaviour like icons, themes, desktop app...


## Links

&nbsp; | Official Links
---------|-------
Website | [d3m0n.4re5.com](https://d3m0n.4re5.com/)
Discord | [discord.com](https://discord.com/invite/Rfb5ajz7cM)
Subreddit | [r/d3m0nproject](https://www.reddit.com/r/d3m0nproject/)

## Made with ❤️ by 4RE5's devs
<br>
<a href="https://github.com/d3m0n-project/d3m0n_os/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=d3m0n-project/d3m0n_os" />
</a>

