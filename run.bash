#!/bin/bash
#----------------------------------------
#\file run.bash
#\brief A script to process root files produced with the ddasdumper 
#\author S. V. Paulauskas
#\date 03-24-2014
#----------------------------------------

#---------- Some default values ----------
rawLoc="fls"
outLoc="data"

log="scan.log"
if [ -f $log ]
then
    rm $log
fi

function usage {
    echo -e "Usage: ./run.bash OPTIONS"
    echo -e "NOTE: Either \"-r\" or \"-l\" must be included!!"
    echo -e "Option List :" 
    echo -e "-f VAR : specify the folder for input"
    echo -e "-o VAR : specify the folder for output"
    echo -e "-r VAR : specify the run number to analyze"
    echo -e "-l VAR : specify a run list"
    echo -e "-t VAR : specify that this is a testing file where VAR is the number"
}

while getopts f:o:l:r:t: opt
do 
    case $opt in 
        f)
            rawLoc=$OPTARG
            ;;
        o)
            outLoc=$OPTARG
            ;;
        l)
            list=$OPTARG
            ;;
        r)
            runNum=$OPTARG
            ;;
        t)
            testNum=$OPTARG
            testRun="true"
            outLoc+="test"
            ;;
        \?)
            usage
            exit 1
            ;;
    esac
done

if [[ ( -z $list ) && ( -z $runNum ) && ( -z $testRun ) ]]
then
    usage
    exit 1
elif [[ ( -z $runNum ) && ( ! -z $testRun ) ]]
then
    usage
    exit 1
else
    #---------- Output Default values ---------
    echo "Here are the options for this run:"
    echo "Raw Root File Location : $rawLoc"
    echo "Output File Location : $outLoc"
    if [ ! -z $testRun ]
    then
        echo "Test Run: $testNum"
    fi
    if [ ! -z $runNum ] 
    then
        echo "Run Number : $runNum"
        runList=$runNum
    fi
    
    if [ ! -z $list ]
    then
        if [ -f $list ]
        then
            echo "Run List : $list"
            runList="$runList `cat $list`"
        else
            echo "Oh man, the list you specified doesn't actually exist!!"
            echo "Exiting..."
            exit 1
        fi
    fi
    
    #--------- Check that things are good for the user ---------
    echo -e "\nAre these options good for you? (y or n)"
    read ans
    case $ans in
        [yY]* )
            echo "Autobots....roll out!"
            ;;
        [nN]* )
            echo -e "Well, I suggest you restart! Exiting..."
            exit 1
            ;;
        *) 
            echo "Invalid answer, use y or n"
            ;;
    esac
fi

for i in $runList
do
    if [ $testRun -e "true" ]
    then
        if [ ! -d "$outLoc" ]
        then
            echo "Created folder: $outLoc" >> $log
            mkdir $outLoc
        fi
        ./he6scan $rawLoc/run$i/*.root $outLoc/test$testNum.root
        
    else
        if [ ! -d "$outLoc/run$i" ]
        then
            echo "Created folder: $outLoc/run$i" >> $log
            mkdir $outLoc/run$i
        fi
        
        name="$outLoc/run$i/run$i-p.root"
        
        if [ -f $name ]
        then
            echo "$name Exists! Skipping...." >> $log
        else
            echo "Created : $name" >> $log
            ./he6scan $rawLoc/run$i/*.root $name
        fi
    fi
done