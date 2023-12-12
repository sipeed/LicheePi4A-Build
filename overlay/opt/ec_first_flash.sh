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
EC_RST="505"

set -ux

echo $EC_BOOT > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio$EC_BOOT/direction
echo $EC_RST > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio$EC_RST/direction

boot_high() {
	echo 1 > /sys/class/gpio/gpio$EC_BOOT/value
}

boot_low() {
	echo 0 > /sys/class/gpio/gpio$EC_BOOT/value
}

rst_high() {
	echo 0 > /sys/class/gpio/gpio$EC_RST/value
}

rst_low() {
	echo 1 > /sys/class/gpio/gpio$EC_RST/value
}

boot_high
sleep 0.5
boot_low
sleep 0.5
rst_low
sleep 0.5
rst_high
sleep 0.5
boot_high
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
else
	echo "ec flash failed"
	echo "ec flash failed"
	echo "ec flash failed"
	echo "ec flash failed"
	echo "ec flash failed"
fi
sleep 10000
