WEB INTERFACE FOR RASPBERRY PI

Python + Flask + Bootstrap CSS

Shows all files in /home/pi/files. Allows to create or upload new ones. Can be tested on any GNU/Linux machine.

---------------------------------

Project structure:

app.py - main script
settings.py - global settings (path to files folder, by default "/home/pi/files")
util.py - utility fonctions 

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



To see the interface:

hostname -I
Paste the address into a web broswer.