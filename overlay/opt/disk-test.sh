#!/bin/bash

set -x
sudo sh /opt/ssd-power-cycle.sh
sleep 1

if (cat /proc/device-tree/model | grep LicheeConsole4A);
then
	SD_DEV="/dev/mmcblk1"
	SSD_DEV="/dev/sda"
else
	SD_DEV="/dev/sdb"
	SSD_DEV="/dev/sda"
fi

BLOCK_SIZE=512
BLOCKS=204800 # 100MiB

sudo badblocks -v $SD_DEV $BLOCKS

if [ "$?" = "0" ]
then
	echo "sd test ok"
	echo "sd test ok"
else
	echo "sd test failed"
	echo "sd test failed"
	sleep 10000
fi

sudo badblocks -v $SSD_DEV $BLOCKS
if [ "$?" = "0" ]
then
	echo "sd & ssd test ok"
	echo "sd & ssd test ok"
else
	echo "ssd test failed, sd test ok"
	echo "ssd test failed, sd test ok"
fi
sleep 10000
