#! /bin/bash

if [[ -z "$3" ]] #its the last one so if something before is null this will also be null, no need to check the other arguments for null.
then
    echo Error missing arguments
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "Error $1 does not exist."
    exit 1
fi

if [  -d "$2" ]; then
    echo "Error $2 Already exists."
    exit 1
fi

if [  $3 -lt 1 ]; then
    echo Error on numFilesPerDirectory argument
    exit 1
fi

mkdir "$2"

declare -A country_array
k=0
while IFS=' ': read -r f1 f2 f3 f4 f5 f6 f7 f8
do
    if [[ -z "${country_array["$f4"]}" ]]
    then
        mkdir ""$2"/"$f4""
        for ((i = 1 ; i <= $3 ; i++))
        do
            touch ""$2"/"$f4"/"$f4"-"$i".txt"
        done
        echo "$f1 $f2 $f3 $f4 $f5 $f6 $f7 $f8" >> "$2"/"$f4"/"$f4"-"1".txt
        country_array[$f4]=2
    else
        if [[ "${country_array["$f4"]}" -gt "$3" ]]
        then
            country_array["$f4"]=1
        fi
        echo "$f1 $f2 $f3 $f4 $f5 $f6 $f7 $f8" >> "$2"/"$f4"/"$f4"-"${country_array["$f4"]}".txt
        country_array["$f4"]="$((${country_array["$f4"]}+1))"
    fi
done < "$1"