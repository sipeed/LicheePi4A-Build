#!/bin/bash

. /opt/ansi_color.sh

echo "如果下载长时间卡住，则测试失败"
/opt/busybox wget -O /dev/null http://192.168.0.104:8000/rand100MiB
if [ "$?" = "0" ]
then
	green
	echo "eth test ok"
	echo "有线网络测试通过"
	nocolor
else
	green
	echo "eth test failed"
	echo "有线网络测试失败"
	nocolor
fi
sleep 100000
