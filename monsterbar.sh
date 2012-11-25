#!/bin/sh

f=/tmp/m
rm -f $f
mkfifo -m600 $f

./mstatusbar < $f | bar &

monsterwm > $f

rm -f $f
