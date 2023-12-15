#!/bin/bash

. /opt/ansi_color.sh 

sudo nmcli radio wifi on
sudo nmcli dev wifi list | cat
sudo nmcli dev wifi connect Sipeed_Guest password 'qwert123'
echo "start wifi test, if wget hang, please close it then restart test"
echo "开始WIFI测试，如果下载卡住，请重启测试脚本"
/opt/busybox wget -O /dev/null http://192.168.0.104:8000/rand100MiB
if [ "$?" = "0" ]
then
	green
	echo "wifi test ok"
	echo "无线网络测试通过"
	nocolor
else
	red
	echo "wifi test failed"
	echo "无线网络测试失败"
	nocolor
fi
sleep 100000
