#!/bin/bash

rm /home/sipeed/.config/autostart/DisplaySetup.desktop
# my work is toooo busy, haven't time merge some change into upstream
# lock /boot/extlinux/extlinux.conf
sudo apt purge u-boot-menu -y
if (cat /proc/device-tree/model | grep LicheeConsole4A);
then
	sh /opt/console-setup.sh
else
	sh /opt/14inch-m0-setup.sh
fi
echo 'touch setup done'
