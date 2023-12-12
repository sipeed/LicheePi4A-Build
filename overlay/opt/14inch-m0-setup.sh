#!/bin/sh

cp /opt/cst3240.service /etc/systemd/system/
systemctl enable cst3240
systemctl start cst3240
echo "z14inch-m0 setup done"
