#!/bin/sh

set -x

sudo nmcli radio wifi on
sudo nmcli dev wifi list | cat
sudo nmcli dev wifi connect Sipeed_Guest password 'qwert123'
/opt/busybox wget -O /dev/null http://192.168.0.104:8000/rand100MiB
if [ "$?" = "0" ]
then
	echo "wifi test ok"
	echo "wifi test ok"
else
	echo "wifi test failed"
	echo "wifi test failed"
fi
sleep 100000
