#!/bin/bash

echo "KEY_DISPLAYTOGGLE 是霍尔报告的按键"
echo "如果你关闭笔记本盖子然后再打开的时候看到它，说明霍尔是没有问题的"
echo "测试键盘排线请请依次按下:"
echo "LShift Z X C V B N M < > ? RShift"
echo "屏幕上会报告对应的键值，如果没有，则排线连接有问题"
echo "按回车开始测试:"
read line
sleep 1
hostname >> ~/test.log
echo "keyboard:" >> ~/test.log
sudo /opt/evtest |& tee -a ~/test.log
