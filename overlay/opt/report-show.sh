#!/bin/bash

. /opt/ansi_color.sh

echo "hostname:"
echo "主机名:"
cat ~/test.log | grep lpi4a | sort | uniq | tail -n 1 | tee $HOME/summary.log

green
echo "test pass:"
echo "测试通过:"
cat ~/test.log | grep ok | sort | uniq | tee -a $HOME/summary.log
cat ~/test.log | grep done | sort | uniq | tee -a $HOME/summary.log
nocolor

red
echo "test failed:"
echo "测试失败:"
cat ~/test.log | grep fail | sort | uniq | tee -a $HOME/summary.log
nocolor

echo "按回车显示二维码:"
read line

cat $HOME/summary.log | /opt/qrencode -t UTF8
sleep 10000
