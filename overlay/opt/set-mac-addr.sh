#!/bin/sh

set -e -u

hex2mac() {
	MAC=""
	seq 2 2 12 | while read i
	do
		echo -n ${1} | head -c $i | tail -c 2
		if [ $i != 12 ]
		then
			echo -n ':'
		fi
	done | tr '[:upper:]' '[:lower:]'
}

INFO="${1}"
NAME=$(echo $INFO | awk -F'-' '{print $1}')
MAC0HEX=$(echo $INFO | awk -F'-' '{print $3}')
MAC1HEX=$(echo $MAC0HEX 1 | /opt/busybox dc -e '16o16i?+p')

if [ "$NAME" != "LM4A0" ]
then
	echo "BAD INPUT"
	exit 1
fi

MAC0=$(hex2mac $MAC0HEX)
MAC1=$(hex2mac $MAC1HEX)

echo "end0: $MAC0"
echo "end1: $MAC1"

/opt/fw_setenv ethaddr $MAC0
/opt/fw_setenv eth1addr $MAC1

/opt/fw_printenv ethaddr
/opt/fw_printenv eth1addr

# hostname with mac address
OLD_HOSTNAME=$(cat /etc/hostname)
NEW_HOSTNAME="lpi4a$(echo $MAC0 | tr -d ':\n' | tail -c 4)"

for file in /etc/hostname /etc/hosts
do
	sed -i -e "s/$OLD_HOSTNAME/$NEW_HOSTNAME/g" $file
done
nmcli general hostname "$NEW_HOSTNAME"
hostname "$NEW_HOSTNAME"

echo "mac address change ok"
