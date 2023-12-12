#!/bin/bash

rm /home/sipeed/.config/autostart/DisplaySetup.desktop
if (cat /proc/device-tree/model | grep LicheeConsole4A);
then
	sh /opt/console-setup.sh
else
	sh /opt/14inch-m0-setup.sh
fi
echo 'touch setup done'
