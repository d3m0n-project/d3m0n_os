UPLOAD_FOLDER="apps"
#UPLOAD_FOLDER="display"

cd $UPLOAD_FOLDER

7z a -r ./test-export.7z *

# FOLDER="d3m0n-testing/"
FOLDER="d3m0n-testing/$UPLOAD_FOLDER"

command="sudo rm -rf d3m0n.exe && sudo bash ./build.sh"
# command="sudo rm -rf display.exe && sudo mcs *.cs ./types/*.cs -out:display.exe -r:System.Drawing.dll -r:System.Windows.Forms.dll && sudo mono display.exe"

local_ip="192.168.43.130"

# ssh d3m0n@192.168.43.65 "rm -rf test-export.7z && wget $local_ip:8000/test-export.7z && sudo 7z x -r -aoa test-export.7z -o/home/d3m0n/$FOLDER"

ssh d3m0n@192.168.43.65 "rm -rf test-export.7z && wget $local_ip:8000/$UPLOAD_FOLDER/test-export.7z && sudo 7z x -r -aoa test-export.7z -o/home/d3m0n/$FOLDER"
clear
ssh d3m0n@192.168.43.65 "pwd && echo $FOLDER && cd /home/d3m0n/$FOLDER && $command"