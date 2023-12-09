7z a display.7z *

ssh -X d3m0n@192.168.43.65 "wget 192.168.43.89:8000/display.7z && sudo 7z x -r -aoa display.7z -o /home/d3m0n/testing_display"

gcc -shared -o lib.so -fPIC ili9340.c fontx.c xpt2046.c ../bcm2835-1.56/src/bcm2835.c
echo "Compiled C lib successfully!"
nano lib.cs
mcs lib.cs
sudo mono ./lib.exe