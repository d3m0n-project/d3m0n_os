#!/bin/bash 
# d3m0n_os startup object (executed in /etc/rc.local)

printf "\n"
printf "\n"
printf "[  \033[0;32mOK\033[0m  ] d3m0n started successfully\n"
printf "\n"
printf "\n"

bash /usr/share/d3m0n/display/run.sh

exit 0