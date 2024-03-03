#!/bin/bash

echo "TO USE THIS SCRIPT, COPY IT TO APP's SOURCE PARENT DIRECTORY AND RUN IT USING ./build.sh"

TEMP=$(pwd)
CURRENT_DIR=$(basename $TEMP)

echo -e "\e[32m		 █████╗ ██████╗ ██████╗     ██████╗ ██╗   ██╗██╗██╗     ██████╗ ███████╗██████╗ 
		██╔══██╗██╔══██╗██╔══██╗    ██╔══██╗██║   ██║██║██║     ██╔══██╗██╔════╝██╔══██╗
		███████║██████╔╝██████╔╝    ██████╔╝██║   ██║██║██║     ██║  ██║█████╗  ██████╔╝
		██╔══██║██╔═══╝ ██╔═══╝     ██╔══██╗██║   ██║██║██║     ██║  ██║██╔══╝  ██╔══██╗
		██║  ██║██║     ██║         ██████╔╝╚██████╔╝██║███████╗██████╔╝███████╗██║  ██║
		╚═╝  ╚═╝╚═╝     ╚═╝         ╚═════╝  ╚═════╝ ╚═╝╚══════╝╚═════╝ ╚══════╝╚═╝  ╚═╝"
echo -e "		                         \e[35md3m0n application builder\e[0m"

echo -e "\e[33m
Current path: ${CURRENT_DIR}\e[0m"


cd source
zip -r ${CURRENT_DIR}.d3m0n *

cd ..

cp -rf source/${CURRENT_DIR}.d3m0n ./
rm -rf source/${CURRENT_DIR}.d3m0n
