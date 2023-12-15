#!/bin/bash

. /opt/ansi_color.sh

echo "开始磁盘测试"
echo "disk test start"
if (cat /proc/device-tree/model | grep LicheeConsole4A);
then
	SD_DEV="/dev/mmcblk1"
	SSD_DEV="/dev/sda"
else
	SD_DEV="/dev/sda"
	SSD_DEV="/dev/sdb"
fi

if [ ! -e "$SSD_DEV" ]
then
	sudo sh /opt/ssd-power-cycle.sh
	sleep 1
fi

BLOCK_SIZE=512
BLOCKS=204800 # 100MiB

hostname >> ~/test.log
echo "sdcard:" >> ~/test.log
sudo badblocks -v $SD_DEV $BLOCKS >> ~/test.log

if [ "$?" = "0" ]
then
	green
	echo "sd test ok" >> ~/test.log
	echo "sd卡测试通过"
	nocolor
else
	red
	echo "sd test failed" >> ~/test.log
	echo "sd卡测试失败"
	nocolor
	sleep 10000
fi

hostname >> ~/test.log
echo "ssd:" >> ~/test.log
sudo badblocks -v $SSD_DEV $BLOCKS >> ~/test.log
if [ "$?" = "0" ]
then
	green
	echo "sd & ssd test ok" >> ~/test.log
	echo "sd卡和ssd测试通过"
	nocolor
else
	red
	echo "ssd test failed, sd test ok" >> ~/test.log
	echo "sd卡测试通过，但是ssd测试失败"
	nocolor
fi
sleep 10000
