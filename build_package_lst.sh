#!/usr/bin/env bash

set -e

if [[ -z $1 ]]; then
    echo -e "\e[31mERROR\e[0m: Usage: $0 path/to/apps/" >&2
    exit 1
fi

APPS_PATH="$1"

echo "Generating package.lst for path $APPS_PATH..."

rm -f "$APPS_PATH/packages.lst"
touch "$APPS_PATH/packages.lst"

find "$APPS_PATH" -type f -name "app" | while IFS= read -r path; do
    echo "${path#"$APPS_PATH"}"
done >> "$APPS_PATH/packages.lst"