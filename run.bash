#!/bin/bash

otFl="/mnt/analysis/e13504/svp/rootFiles/run145/run-0145-summed.root"

if [ -e "$otFl" ]
then
    echo "Removing " $otFl
    rm $otFl
fi

make clean && make && ./ex 