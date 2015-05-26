#!/bin/bash

dir="data"
#file=$dir/
file="/mnt/daqtesting/funix/poldev/processed/run88"

for i in $file/*
do
    make && ./scan $i $dir/test00.root
done