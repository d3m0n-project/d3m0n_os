FOLDER="./d3m0n-testing/display/"
command="sudo rm -rf display.exe && sudo mcs *.cs ./types/*.cs -r:System.Drawing.dll -r:System.Windows.Forms.dll && sudo mono display.exe"

local_ip="192.168.43.130"
echo $1

ssh -X d3m0n@192.168.43.65 "sudo rm -rf $FOLDER/$1 && sudo wget $local_ip:8000/$1 && sudo cp $(basename $1) $FOLDER/$1"
# clear
ssh -X d3m0n@192.168.43.65 "cd $FOLDER && $command"