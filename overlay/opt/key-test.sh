#!/bin/bash

echo "KEY_DISPLAYTOGGLE is hall event"
echo "if you see it, hall is ok"
echo "KEY_DISPLAYTOGGLE 是霍尔报告的按键"
echo "如果你关闭笔记本盖子然后再打开的时候看到它，说明霍尔是没有问题的"
sleep 1
sudo /opt/evtest 
