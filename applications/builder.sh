#!/usr/bin/env bash

set -e
ROOTFS_APP_PATH="$1"

printf "============================== APPS BUILDER ============================\n"
printf "\033[32m[BUILD] Started Builder...\033[0m\n"
find ./applications -type f -name "Makefile" -execdir sh -c 'make export APP_PATH="$1" 1>/dev/null || exit 1' _ "$ROOTFS_APP_PATH" \; || exit 1

printf "\033[32m[BUILD] Builder Finished Successfully...\033[0m\n"
printf "========================================================================\n\n"