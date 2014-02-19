#!/bin/bash

make clean && make

for i in 133 143 145 146 147 148 153
do
    if [ ! -d "data/run$i" ]
    then
        mkdir data/run$i
    fi
    outFile="data/run$i/run-0$i-summed.root"
    #outFile="data/test/test00.root"
    
    ./he6scan fls/run$i/*.root $outFile
done