#!/bin/bash 

menu() {
	echo -e -n "
	    \e[33m██████╗ ██████╗ ███╗   ███╗ ██████╗ ███╗   ██╗
	    ██╔══██╗╚════██╗████╗ ████║██╔═████╗████╗  ██║
	    ██║  ██║ █████╔╝██╔████╔██║██║██╔██║██╔██╗ ██║
	    ██║  ██║ ╚═══██╗██║╚██╔╝██║████╔╝██║██║╚██╗██║
	    ██████╔╝██████╔╝██║ ╚═╝ ██║╚██████╔╝██║ ╚████║
	    ╚═════╝ ╚═════╝ ╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═══╝
	                  \e[31mmade by \e[32m4re5 group                  
	           \e[4;31mthe first hacking cellular phone\e[0m

	        \e[93m[1] Build d3m0n_os image\e[0m
	        \e[93m[2] Configurate OS\e[0m
	        \e[93m[99] Exit\e[0m

\e[93mEnter selected number to access d3m0n: \e[0m"

	read input
	if [[ $input == '1' ]]; then
		./build.sh
	elif [[ $input == '2' ]]; then
		./config
	elif [[ $input == '99' ]]; then
		exit 0
	else
		echo -e "\e[3;35m[x] Invalid option (99 or C^ to exit)\e[0m"
	fi
}

menu