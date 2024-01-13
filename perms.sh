#!/bin/bash

ROOTFS_DIR=$1
BOOTFS_DIR=$2

cd $ROOTFS_DIR

symlink_mappings=(
  "bin:/usr/bin"
)

# Loop through the mappings and create symlinks
for mapping in "${symlink_mappings[@]}"; do
  source_path="${mapping%%:*}"
  target_path="${mapping#*:}"

  if [ -e "$source_path" ]; then
    if [ -L "$source_path" ]; then
      echo "Symlink $source_path already exists."
    else
      echo "Creating symlink $source_path -> $target_path"
      ln -s "$target_path" "$source_path"
    fi
  else
    echo "Source path $source_path does not exist. Skipping."
  fi
done