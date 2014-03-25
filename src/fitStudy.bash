#!/bin/bash

lowInit=500
hiInit=5000
step=100

file="src/fitting.C"
tmp="src/tmp.C"
log="fitStudy.log"

for i in `seq $lowInit 100 1000`
do
    low=$i
    for j in `seq $hiInit 100 7500`
    do
        hi=$j
        newRange="const double fLow = $low, fHigh = $hi;"
        sed '20c\'"$newRange" $file > $tmp
        mv $tmp $file
        echo -e "We are working on fitting range : $i $j\n"
        echo -e "We are working on fitting range : $i $j\n" >> fitStudy.log
        root -b $file > /dev/null &2>> fitStudy.log
    done
    echo -e "\n" >> data/fitstudy/cal-wF.dat
done
