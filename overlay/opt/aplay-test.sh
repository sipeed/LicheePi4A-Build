#!/bin/sh

echo "mic test, please make some sound"
echo "内置麦克风测试，请敲击笔记本外壳"
sudo arecord -Dhw:0,1 -d 3 -r 48000 -f S16_LE -t wav /tmp/rec.wav
echo "now play your record"
echo "现在播放板载麦克风录制的声音，如果没有声音，请重试，重试2次仍然无声音，则标记为声音问题"
sudo aplay /tmp/rec.wav

echo "please connect 3.5mm, then press enter"
echo "请连接3.5mm的 麦克风+喇叭,然后按回车"
read line
echo "now record sound from 3.5mm"
echo "开始录制3.5mm的声音"
sudo arecord -Dhw:0,1 -d 3 -r 48000 -c 2 -f S16_LE -t wav /tmp/rec.wav
echo "now play your record"
echo "现在播放刚才录制的声音，如果没有声音，请重试，重试2次仍然无声音，则标记为声音问题"
sudo aplay /tmp/rec.wav

sudo rm /tmp/rec.wav
sleep 10000
