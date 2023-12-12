#!/bin/sh

echo "usb stack restart, please wait"
rmmod uvcvideo
rmmod usbhid
rmmod xhci-plat-hcd
rmmod xhci-hcd
rmmod usbcore
sleep 5
modprobe usbcore
modprobe xhci-hcd
modprobe xhci-plat-hcd
modprobe usbhid
modprobe uvcvideo
