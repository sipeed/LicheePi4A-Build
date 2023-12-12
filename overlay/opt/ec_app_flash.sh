#!/bin/sh

PATH=/opt:$PATH
export PATH

if !(which wchisp);
then
	echo "wchisp not found"
	exit 1
fi

if [ "$1" = "" ]
then
	echo "usage: $0 firmware"
	exit 1
fi

EC_BOOT="504"

set -ux

echo $EC_BOOT > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio$EC_BOOT/direction

boot_high() {
	echo 1 > /sys/class/gpio/gpio$EC_BOOT/value
}

boot_low() {
	echo 0 > /sys/class/gpio/gpio$EC_BOOT/value
}

boot_high
sleep 13
wchisp config reset
CONFIG_RET="$?"
wchisp flash $1
FLASH_RET="$?"
boot_low

if [ "$CONFIG_RET" = "0" ]
then
	echo "ec flash done"
	echo "ec flash done"
	echo "ec flash done"
	echo "ec flash done"
	echo "ec flash done"
	echo "ec flash done"
	sleep 100000
else
	echo "ec flash failed"
	echo "ec flash failed"
	echo "ec flash failed"
	echo "try other method"
	sh /opt/ec_first_flash.sh $1
fi
