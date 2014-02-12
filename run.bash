#!/bin/bash

make clean && make

for i in 145
do
    #outFile="data/run$i/run-0$i-summed.root"
    outFile="data/test/test00.root"

    if [ -e "$otFl" ]
    then
        echo "Removing " $otFl
        rm $otFl
    fi
    ./he6scan fls/run$i/*.root $outFile
done