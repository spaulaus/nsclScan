#!/bin/bash

lowInit=300
hiInit=2300
step=100

file="src/fitting.C"
tmp="src/tmp.C"
log="fitStudy.log"

if [ -f $log ]
then
    rm $log
fi

for i in `seq $lowInit 100 700`
do
    low=$i
    for j in `seq $hiInit 100 5000`
    do
        hi=$j
        newRange="const double fLow = $low, fHigh = $hi;"
        sed '19c\'"$newRange" $file > $tmp
        mv $tmp $file
        echo -e "We are working on fitting range : $i $j\n"
        echo -e "We are working on fitting range : $i $j\n" >> $log
        root -b $file 1>/dev/null 2>>$log
    done
    echo -e "\n" >> data/fitstudy/cal-wF.dat
done
