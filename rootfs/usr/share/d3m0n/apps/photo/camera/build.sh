#!/usr/bash

OUTPUT_FILE="camera.exe"

rm -rf $OUTPUT_FILE
rm -rf include.txt

CURRENT_PATH=$(pwd)

echo "Current path: ${CURRENT_PATH}"

find . -type f -exec echo {} >> "${CURRENT_PATH}/include.txt" \;

FILES_TO_INCLUDE=""

while read file; do
  	if [[ $file == *.cs ]]; then
  		sudo chmod +x $file
		FILES_TO_INCLUDE="${FILES_TO_INCLUDE} ${file}"
	fi
done <include.txt


echo $FILES_TO_INCLUDE

sudo chmod +x *.cs
echo -e "\e[32m[~] Building apps...\e[0m"

mcs -out:${OUTPUT_FILE} -r:System.Windows.Forms.dll -r:System.Drawing.dll ${FILES_TO_INCLUDE}

echo -e "\e[32m[o] Build successfully\e[0m"

mono $OUTPUT_FILE