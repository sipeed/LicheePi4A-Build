#!/bin/bash

. /opt/ansi_color.sh

echo "如果长时间卡住，则测试失败"
hostname >> ~/test.log
sudo nmcli device up end0
sudo nmcli radio wifi off
echo "eth:" >> ~/test.log
time -- /opt/busybox wget -O /dev/null http://192.168.0.104:8000/rand100MiB >> ~/test.log
if [ "$?" = "0" ]
then
	green
	echo "eth test ok" >> ~/test.log
	echo "有线网络测试通过"
	nocolor
else
	red
	echo "eth test failed" >> ~/test.log
	echo "有线网络测试失败"
	nocolor
fi
sleep 100000
