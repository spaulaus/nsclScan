#!/bin/bash

make clean

for i in 145
do
    #if [ ! -d "data/run$i" ]
    #then
    #    mkdir data/run$i
    #fi
    #outFile="data/run$i/run0$i-summed.root"
    outFile="data/test/test00.root"
    
    make && ./he6scan fls/run$i/*.root $outFile
done