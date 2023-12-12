#!/bin/sh

sudo cp kexec /usr/bin/
sudo cp vmcore-dmesg /usr/bin/
sudo cp kdump-tools /usr/bin/
sudo cp /opt/kdump-conf /etc/default/
sudo mkdir -p /var/lib/kdump/
sudo cp /opt/rootfs.cpio.gz /var/lib/kdump/
sudo cp /opt/vmlinux /var/lib/kdump/
sudo cp /opt/kdump.service /etc/systemd/system/
sudo systemctl enable kdump.service
sudo systemctl start kdump.service
