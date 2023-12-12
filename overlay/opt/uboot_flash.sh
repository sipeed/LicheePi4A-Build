#!/bin/sh

set  -e -u

echo 0 > /sys/block/mmcblk0boot0/force_ro
if [ ! -e "$1" ]
then
	exit 1
fi
dd if=$1 of=/dev/mmcblk0boot0 conv=sync
sync
sync
sync
echo "uboot flash done"
