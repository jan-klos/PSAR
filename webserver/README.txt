WEB INTERFACE FOR RASPBERRY PI

Python + Flask + Bootstrap CSS

Shows all files in /home/pi/files. Allows to create or upload new ones. Can be tested on any GNU/Linux machine.

---------------------------------

Project structure:

app.py - main script
settings.py - global settings (paths, extensions)
messages.py - error and confirmation messages
util.py - utility fonctions 
interface_service.sh - script to run it as a service

templates/ - html templates
templates/common/ - shared html code

static/ - basically files visible for templates
static/styles/custom.css - css tweaks
static/tmp/ - folder where images and pdf files are temporarily stored in order to display them (automatically created)

---------------------------------

Dependencies:

Flask
sudo apt-get install python-pip
sudo pip install flask

---------------------------------

To activate the interface:

sudo python webserver/app.py

Alternatively, to make it run as service:

chmod 755 PSAR/webserver/app.py
cp PSAR/webserver/interface_service.sh /etc/init.d/
chmod 755 /etc/init.d/interface_service.sh
update-rc.d interface_service.sh defaults
reboot

It can be verified with the following command:
service interface_service.sh status

http://blog.scphillips.com/posts/2013/07/getting-a-python-script-to-run-in-the-background-as-a-service-on-boot/

--------------------------------

To see the interface:

hostname -I
Paste the address into a web broswer (or just 0.0.0.0 if locally).