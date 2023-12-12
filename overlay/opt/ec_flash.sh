#!/bin/sh

if (cat /proc/device-tree/model | grep LicheeConsole4A);
then
	FW="/opt/ch582m_console.bin"
else
	FW="/opt/ch582m_z14inch_m0.bin"
fi


sudo sh /opt/ec_app_flash.sh $FW
