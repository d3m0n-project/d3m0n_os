#!/bin/bash 
# d3m0n_os startup object (executed in /etc/rc.local)

# optimize d3m0n phone
# ref https://www.cnx-software.com/2021/12/09/raspberry-pi-zero-2-w-power-consumption/
echo none | sudo tee /sys/class/leds/led0/trigger
echo 1 | sudo tee /sys/class/leds/led0/brightness


# setup display
#sudo xinit &
#export DISPLAY=:0

# xrandr --output default --mode 640x480

# setup TFT screen driver
sudo bash /usr/share/d3m0n/display/run.sh &

# create zram
sudo chmod +x /usr/share/d3m0n/zram.sh
bash /usr/share/d3m0n/zram.sh &

# display infos on tty1
printf "\n"
printf "\n"
printf "[  \033[0;32mOK\033[0m  ] d3m0n started successfully\n"

# print IP address
_IP=$(hostname -I) || true
if [ "$_IP" ]; then
  printf "Current IP address is %s\n" "$_IP"
fi
print "\n"
printf "\n"
printf "\n"


# display image
# sudo fbi -d /dev/fb0 -T 1 --fitwidth --noverbose splash.png

# manually setup command
# sudo apt install p7zip-full -y && wget 192.168.43.89:8000/d3m0n.7z && sudo 7z x -r -aoa d3m0n.7z -o/usr/share/d3m0n && sudo bash /usr/share/d3m0n/firstboot.sh

# sudo X -nocursor -s 0 -dpms&



# setup prompt
color=16
length=${#user}
_user=""
for ((i = 0; i < length; i++)); do
		char="${user:i:1}"
		_user=$_user+"\033[01;38;5;${color}m$char"
		((color++));
done
# hostname
color=16
length=${#hostname}
for ((i = 0; i < length; i++)); do
		char="${hostname:i:1}"
		_host=$_host+"\033[01;38;5;${color}m$char"
		((color++));
done
d="\[\033[01;38;5;52m\]p"
3="\[\033[01;38;5;124m\]l"
m="\[\033[01;38;5;196m\]a"
0="\[\033[01;38;5;202m\]s"
n="\[\033[01;38;5;208m\]m"
# a2="\[\033[01;38;5;214m\]a"
# r="\[\033[01;38;5;220m\]r"
# o="\[\033[01;38;5;226m\]o"
# b="\[\033[01;38;5;228m\]b"
#local _user="$d$3$m$0$n"
#local _host="$"

export PS1="┌──($_user@$_host)-[$cd]
└─# "
# ┌──────┐
# │      │
# └──────┘

#export DISPLAY=:0
#sudo mono /usr/share/d3m0n/apps/d3m0n.exe&


# sudo xvfb-run -n 0 -s "-screen 0 480x640x16" sudo mono /usr/share/d3m0n/apps/d3m0n.exe
# sudo xvfb-run --auto-servernum --server-num=1 sudo mono d3m0n.exe &

# screenshot sudo xwd -display :0 -root -silent | convert xwd:- png:/tmp/screenshot.png
# display build cmake ../ -DILI9341=ON -DGPIO_TFT_DATA_CONTROL=24 -DGPIO_TFT_RESET_PIN=25 -DSPI_BUS_CLOCK_DIVISOR=6 -DSTATISTICS=number

exit 0