#!/usr/bin/env bash

set -e

export ROOTFS_APP_PATH="$1"

printf "============================== APPS BUILDER ============================\n"
printf "\033[32m[BUILD] Started Builder...\033[0m\n"

while IFS= read -r makefile; do
    path="${makefile#./applications/}"
    path="${path%/Makefile}"

    printf "\033[1;36mBUILDING\033[0;36m:  $makefile\033[0m\n"

    app_dir="${makefile%/Makefile}"
    app_output_root="$(realpath --relative-to="$app_dir" "$ROOTFS_APP_PATH")"
    make -C "$app_dir" export APP_PATH="$app_output_root/$path/" 1>/dev/null

    printf "\n"
done < <(find ./applications -mindepth 2 -maxdepth 3 -type f -name "Makefile")

printf "\033[32m[BUILD] Builder Finished Successfully...\033[0m\n"
printf "========================================================================\n\n"
