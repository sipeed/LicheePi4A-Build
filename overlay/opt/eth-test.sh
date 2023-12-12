#!/bin/sh

set -x

/opt/busybox wget -O /dev/null http://192.168.0.104:8000/rand100MiB
if [ "$?" = "0" ]
then
	echo "eth test ok"
	echo "eth test ok"
else
	echo "eth test failed"
	echo "eth test failed"
fi
sleep 100000
