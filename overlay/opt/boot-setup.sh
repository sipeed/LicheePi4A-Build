#!/bin/sh

# set path
PATH=/usr/local/bin:/usr/local/sbin:/usr/bin:/usr/sbin:/bin:/sbin
export PATH

# for LicheeConsole4A
if (cat /proc/device-tree/model | grep LicheeConsole4A);
then
	echo 1 > /sys/class/graphics/fbcon/rotate_all
fi

# bluetooth driver
/usr/bin/hciattach ttyS4 any 1500000

exit 0
