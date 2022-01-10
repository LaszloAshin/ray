#!/bin/sh

for i in `seq 500`; do ../ray $i; mv tracement.tga $(printf "frame-%03d.tga" $i); done
ffmpeg -r 25 -f image2 -s 1280x800 -i frame-%03d.tga -vcodec libx264 -crf 18 -pix_fmt yuv420p test.mp4
