#!/bin/bash

( nice -n 10 -- stress-ng -c4 -i4 -m4 ) &
(
while true
do
	cat /sys/class/thermal/thermal_zone0/temp
	sleep 1
done
)
