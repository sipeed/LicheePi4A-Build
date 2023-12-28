#!/bin/sh

set -u
THIS_DIR=$(dirname $(realpath $0))
if [ "$THIS_DIR" = "/opt" ]
then
	echo "already copy into /opt"
	exit 0
fi
if [ ! -e "$THIS_DIR/../boot" ]
then
	echo "missing boot"
	sleep 10000
fi
if [ ! -e "$THIS_DIR/../etc" ]
then
	echo "missing etc"
	sleep 10000
fi
if [ ! -e "$THIS_DIR/../opt" ]
then
	echo "missing opt"
	sleep 10000
fi

sudo cp -arv $THIS_DIR/../ /
