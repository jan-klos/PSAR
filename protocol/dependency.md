sudo apt-get install libnl-genl-3-dev
sudo apt-get install ntp
sudo service ntp start
sudo unlink /etc/localtime
sudo ln -s /usr/share/zoneinfo/Europe/Paris /etc/localtime

###### Config - changer pour chaque rpi
sudo sed -i 's/raspberrypi/MobRPi2/g' /etc/hosts 