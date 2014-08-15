#!/bin/bash
#----------------------------------------
#\file run.bash
#\brief A script to process root files produced with the ddasdumper 
#\author S. V. Paulauskas
#\date 03-24-2014
#----------------------------------------

#---------- Some default values ----------
rawLoc="/mnt/daqtesting/funix/processed"
outLoc="/mnt/daqtesting/funix/scanned"

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

while getopts f:o:l:r:t:m opt
do 
    case $opt in 
        m)
            make clean
            if ! make
            then
                echo -e "\n The compilation failed. You may want to check that."
                exit 1
            fi
            ;;
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
            outLoc+="/test"
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
            echo -e "\n\nAutobots....roll out!\n\n"
            ;;
        [nN]* )
            echo -e "Well, I suggest you restart! Exiting..."
            exit 1
            ;;
        *) 
            echo "Invalid answer, use y or n"
            exit 1
            ;;
    esac
fi

for i in $runList
do
    if [ `ls $rawLoc/run$i/*.root 2>/dev/null` ]
    then
        echo -e "Found the input files, proceeding.\n\n"
    else
        echo -e "I could not find the input files: $rawLoc/run$i/*.root"
        continue
    fi        

    if [ "$testRun" = "true" ]
    then
        if [ ! -d "$outLoc" ]
        then
            echo "Created folder: $outLoc" >> $log
            mkdir $outLoc
        fi
        ./scan $rawLoc/run$i/*.root $outLoc/test$testNum.root
        
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
            ./scan $rawLoc/run$i/*.root $name
        fi
    fi
done