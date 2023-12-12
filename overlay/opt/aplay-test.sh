#!/bin/sh

echo "mic test, please make some sound"
echo "mic test, please make some sound"
echo "mic test, please make some sound"
echo "mic test, please make some sound"
sudo arecord -Dhw:0,1 -d 3 -r 48000 -f S16_LE -t wav rec.wav
echo "now play your record"
echo "now play your record"
echo "now play your record"
echo "now play your record"
echo "now play your record"
sudo aplay rec.wav

echo "please connect 3.5mm"
echo "please connect 3.5mm"
echo "please connect 3.5mm"
echo "please connect 3.5mm"
echo "please connect 3.5mm"
sleep 3
echo "3.5mm mic test, please make some sound"
echo "3.5mm mic test, please make some sound"
echo "3.5mm mic test, please make some sound"
echo "3.5mm mic test, please make some sound"
echo "3.5mm mic test, please make some sound"
echo "3.5mm mic test, please make some sound"
sudo arecord -Dhw:0,1 -d 3 -r 48000 -c 2 -f S16_LE -t wav rec.wav

echo "please disconnect 3.5mm mic, then connect 3.5mm speaker"
echo "please disconnect 3.5mm mic, then connect 3.5mm speaker"
echo "please disconnect 3.5mm mic, then connect 3.5mm speaker"
echo "please disconnect 3.5mm mic, then connect 3.5mm speaker"
sleep 5
echo "now play your record"
echo "now play your record"
echo "now play your record"
echo "now play your record"
echo "now play your record"
echo "now play your record"
sudo aplay rec.wav

sudo rm rec.wav
sleep 10000
