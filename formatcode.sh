#!/bin/bash

files=($(find src -iname "*.c*" -o -iname "*.h*" ))

for file in ${files[@]}; do
    #echo $file
    astyle --suffix=none -A10 -S -N -w -Y -H -m0 -xC78 -xL -p -U -k3 -W3 -j -c -xy -xp -xw -xW -xy -w -Y -q -z2 -f $file | grep -v "^Unchanged" || true
done
