#!/bin/sh

sudo cp /opt/cst3240.service /etc/systemd/system/
sudo systemctl enable cst3240
sudo systemctl start cst3240
