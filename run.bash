#!/bin/bash

for i in 133 143 144 146 147 148 153
do
    outFile="data/run$i/run-0$i-summed.root"

    if [ -e "$otFl" ]
    then
        echo "Removing " $otFl
        rm $otFl
    fi
    ./ex fls/run$i/*.root $outFile
done