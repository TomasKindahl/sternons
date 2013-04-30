#!/bin/bash
for f in maps/*-v1.mkm; do
	const=`echo $f|sed 's/maps\///'|sed 's/-v1.mkm//'`
	echo Making $const:
	./compile-mkm maps/$const-v1.mkm
	./mkmap maps/$const-v1.mbf
done
