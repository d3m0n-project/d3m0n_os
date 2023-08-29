
# ![icon](https://avatars.githubusercontent.com/u/136185636?s=40&u=76e3b4569d8be273bb0358ae3c71fcdf7c7b2d6b&v=4) d3m0n os (C) 2023 4re5 group

d3m0n OS is a Debian based ARM CPU architecture operating system. 

> :warning: This code is made to run on a d3m0n device only :warning:

d3m0n app system and binaries are located in [rootfs/usr/share/d3m0n](https://github.com/d3m0n-project/d3m0n_os/tree/main/rootfs/usr/share/d3m0n)

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


## Creating a d3m0n application

First create a main file named `app` and type these lines

    name: myapp
    package: com.myteam.myapp
    icon: /path/to/icon.png
    perms: none
    start_path: /path/to
    category: mycategory

### description
`name` is your new app name
`package` is your app 'id'
`icon` is then path to your icon (png) file or a theme icon ([theme-icons](#theme-icons))
`perms` can be equal to `sudo`(admin) or `none`(default)
`start_path` specifiate the directory where your app will be running
`category` is your app category

Then create a main layout file in the directory `layouts` named `main.layout`.

    # d3m0n layout << important!
	
	Window:
		name="My_Window"
		title="My window"
		width="200"
		height="100"
		bg_color="black"
	
	Text:
		content="my text"

create now a main source file in the directory `src` named `main.src`.

    # d3m0n source << important!
	
	# On app load
	Window.OnCreate => 
		# this is a comment
		myVariable="hello world"
		
		# create an alert box with myVariable content
		alert(myVariable);
		
		# write something in the host terminal in orange
		logn("[o] loaded myApp", "orange");
		
		# execute bash command
		system("echo test");
		
		# call a function called "myFunct" with args 'arg1' and 'arg2' in the dll
		[/path/to/myDll.dll,myNamespace,myclass,myFunct(arg1, arg2)]
	

	# on app quit
	Window.OnDestroy => 
		system("echo 'd3m0n path:         $d3m0n'");
		system("echo 'app ressource path: $ressource'");
		system("echo 'app source path:    $source'");
		system("echo 'app layouts path:   $layouts'");


	Element.OnClick => exec="python $source/main.py" 
	
this file will be executed in the same time that your layout one


Finnally export your app by calling the script `app_builder.sh`



# Apparence
Your system design can be changed to be fancier.

## theme icons

Your d3m0n os apparence can be modified by using a theme.
These ones are located in 

> /usr/share/d3m0n/themes

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
