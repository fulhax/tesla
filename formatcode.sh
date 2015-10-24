#!/bin/bash

files=($(find src -iname "*.c*" -o -iname "*.h*" ))

for file in ${files[@]}; do
    #echo $file
    astyle -A3 -S -w -Y -m1 -xC78 -xL -p -U -k3 -W3 -j -c -xy -xp -xw -f $file
done
