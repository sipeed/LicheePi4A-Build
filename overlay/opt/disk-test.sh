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

sudo badblocks -v $SD_DEV $BLOCKS

if [ "$?" = "0" ]
then
	green
	echo "sd test ok"
	echo "sd卡测试通过"
	nocolor
else
	red
	echo "sd test failed"
	echo "sd卡测试失败"
	nocolor
	sleep 10000
fi

sudo badblocks -v $SSD_DEV $BLOCKS
if [ "$?" = "0" ]
then
	green
	echo "sd & ssd test ok"
	echo "sd卡和ssd测试通过"
	nocolor
else
	red
	echo "ssd test failed, sd test ok"
	echo "sd卡测试通过，但是ssd测试失败"
	nocolor
fi
sleep 10000
