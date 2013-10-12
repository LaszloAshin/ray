#!/bin/sh

i=0
while [ $i -lt 250 ]; do
	../raytracer $i
	convert tracement.tga frame-`printf '%03d' $i`.png
	i=$((i + 1))
done
