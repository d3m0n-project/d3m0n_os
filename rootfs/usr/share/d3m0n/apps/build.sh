#!/usr/bash

OUTPUT_FILE="d3m0n.exe"

rm -rf $OUTPUT_FILE
rm -rf include.txt

CURRENT_PATH=$(pwd)

echo "Current path: ${CURRENT_PATH}"

find . -type f -exec echo {} >> "${CURRENT_PATH}/include.txt" \;

FILES_TO_INCLUDE=""

while read file; do
	CURRENT_DIR=$(dirname  ${file})
	# echo "current_dir: ${CURRENT_DIR}"
  	if [[ $file == *.cs ]]; then
  		if [[ -f ${CURRENT_DIR}/app ]]
  		then
  			echo "skipped ${file}"
  		else
  			sudo chmod +x $file
			FILES_TO_INCLUDE="${FILES_TO_INCLUDE} ${file}"
		fi
	fi
	if [[ $file == "./build.sh" ]]; then
		echo "skipped ${file}"
	elif [[ $file == */build.sh ]]; then
		echo "exec: ${file}"
		clear
		cd $CURRENT_DIR
		bash $(basename $file)
		cd $CURRENT_PATH
		echo -e "\e[33m------------------------------------------------------------------------\e[0m"
	fi
done <include.txt


echo $FILES_TO_INCLUDE

sudo chmod +x *.cs
echo -e "\e[32m[~] Building core...\e[0m"

mcs -out:${OUTPUT_FILE} -r:System.Windows.Forms.dll -r:Mono.Posix.dll -r:Mono.WebBrowser.dll -r:System.Drawing.dll ${FILES_TO_INCLUDE}

echo -e "\e[32m[o] Build successfully\e[0m"

mono $OUTPUT_FILE