#!/bin/bash

execFile='./root/main'
mainSourceFile='./root/main.cpp'
sourceFiles=$(find ./root -type f -name '*.[ch]pp')

for file in $sourceFiles
do
    if [ "$file" -nt "$execFile" ]
    then
        printf '%s\n' "$file is newer than $execFile, recompiling"
        g++ -o "$execFile" "$mainSourceFile" 
        break
    fi
done

if [ $? -eq 0 ]; then
    exec -a "$0" $execFile "$@"
fi