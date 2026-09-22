#!/usr/bin/env bash

set -e
export ROOTFS_APP_PATH="$1"

printf "============================== APPS BUILDER ============================\n"
printf "\033[32m[BUILD] Started Builder...\033[0m\n"
find ./applications -mindepth 2 -maxdepth 3 -type f -name "Makefile" -exec sh -c '
    makefile="$1"
    path="${makefile#./applications/}"
    path="${path%/Makefile}"
	printf "\033[1;36mBUILDING\033[0;36m:  $makefile\033[0m\n"
    make -C "${makefile%/Makefile}" export APP_PATH="$ROOTFS_APP_PATH$path/" 1>/dev/null || exit 1
	echo "\n"
' _ {} \; || exit 1



printf "\033[32m[BUILD] Builder Finished Successfully...\033[0m\n"
printf "========================================================================\n\n"