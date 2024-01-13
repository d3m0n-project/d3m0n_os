#!/bin/bash 

color=16;




menu() {
	clear
		
	banner="    ██████╗ ██████╗ ███╗   ███╗ ██████╗ ███╗   ██╗
    ██╔══██╗╚════██╗████╗ ████║██╔═████╗████╗  ██║
    ██║  ██║ █████╔╝██╔████╔██║██║██╔██║██╔██╗ ██║
    ██║  ██║ ╚═══██╗██║╚██╔╝██║████╔╝██║██║╚██╗██║
    ██████╔╝██████╔╝██║ ╚═╝ ██║╚██████╔╝██║ ╚████║
    ╚═════╝ ╚═════╝ ╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═══╝"
	
	length=${#banner}
	for ((i = 0; i < length; i++)); do
		char="${banner:i:1}"
		if [ "$char" = "╝" ] || [ "$char" = "║" ] || [ "$char" = "═" ] || [ "$char" = "╗" ] || [ "$char" = "╚" ] || [ "$char" = "╔" ]; then
			echo -e -n "\033[01;38;5;196m$char"
		else
			#echo -n -e $char
			echo -e -n "\033[01;38;5;01m$char"
		fi
		((color++));
	done

	echo -e -n "
    \\033[38;5;${color}m
                  \e[31mmade by \e[32m4re5 group                  
        \e[4;31mthe first hacking cellular phone\e[0m
        \033[01;38;5;208m[\e[31m1\033[01;38;5;208m]\e[0m Build d3m0n_os image
        \033[01;38;5;208m[\e[31m2\033[01;38;5;208m]\e[0m Build d3m0n app manager
        \033[01;38;5;208m[\e[31m3\033[01;38;5;208m]\e[0m Configure
        \033[01;38;5;208m[\e[31m4\033[01;38;5;208m]\e[0m Install on a blank system
		\033[01;38;5;208m[\e[31m5\033[01;38;5;208m]\e[0m Wiki
        \033[01;38;5;208m[\e[31m99\033[01;38;5;208m]\e[0m Exit

\e[93mEnter selected number to access d3m0n: \e[0m"

	read input
		if [[ $input == '1' ]]; then
			./build.sh
		elif [[ $input == '2' ]]; then
			cd rootfs/usr/share/d3m0n/apps
			bash ./build.sh
		elif [[ $input == '3' ]]; then
			./config
		elif [[ $input == '3' ]]; then
			./install.sh
		elif [[ $input == '99' ]]; then
			exit 0
		else
			echo -e "\e[3;35m[x] Invalid option (99 or C^ to exit)\e[0m"
		fi
}

menu