#!/bin/bash

# d3m0n app remote installer
# usage 'fetch <package name>' inside d3m0n shell

cd ~

FOUND=false
PACKAGE=$1
# debug: 
# GET_PATH="/home/kali/d3m0n_os_debian/rootfs/usr/share/d3m0n"
GET_PATH="/usr/share/d3m0n"

COLOR1="\e[1;35m"
COLOR2="\e[0;35m"

process_file() {
    local json="$1"
    local name=$(echo "$json" | jq -r ".name")
    local download_url=$(echo "$json" | jq -r ".download_url")

    if [[ "$name" == "$PACKAGE" ]]; then
        mkdir -p "$GET_PATH/../applications"
        sudo chmod 777 "$GET_PATH/../applications"
        curl "$download_url" -o "$GET_PATH/../applications/$name"
        echo -e "$COLOR2 Found $COLOR1$PACKAGE$COLOR2 inside d3m0n repository"
        echo -e "$COLOR2 Url: $COLOR1$download_url"
        echo -e "$COLOR2 "
        echo -e "\n\nDownloaded $name successfully!"
        FOUND=true
    fi
}

process_directory() {
    local path="$1"
    local entries=$(curl "https://api.github.com/repos/d3m0n-project/d3m0n_os/contents/$path" | jq -c '.[]')
    # Iterate over each entry
    while IFS= read -r entry; do
        local type=$(echo "$entry" | jq -r '.type')
        local name=$(echo "$entry" | jq -r '.name')

        if [[ "$type" == "file" ]]; then
            process_file "$entry"
        elif [[ "$type" == "dir" ]]; then
            process_directory "$path/$name"
        fi
    done <<< "$entries"
}

# Start processing from the root directory
echo $(pwd)
process_directory "rootfs/usr/share/applications"
if [[ $FOUND == false ]]; then
    echo -e "\e[1;31mPackage $PACKAGE not found in repo"
fi
