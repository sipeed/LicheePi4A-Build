#!/bin/bash

rm /home/sipeed/.config/autostart/DisplaySetup.desktop
sudo cp -rv /etc/skel/.config /home/sipeed/
sudo chown -R sipeed:sipeed /home/sipeed/
sh /opt/display-setup.sh
echo 'console setup done'
