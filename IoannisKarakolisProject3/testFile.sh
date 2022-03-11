#! /bin/bash

if [[ -z "$4" ]] #its the last one so if something before is null this will also be null, no need to check the other arguments for null.
then
    echo Error missing arguments
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "$1 does not exist."
    exit 1
fi

if [ ! -f "$2" ]; then
    echo "$2 does not exist."
    exit 1
fi

if [[ "$3" -lt 1 ]]
then
    echo Error invalid numlines argument.
    exit 1
fi

if [[ "$4" -lt 0 ]]
then
    echo Error invalid dublicatesAllowed argument.
    exit 1
fi

declare -a viruses_array
declare -a countriesarray


mapfile -t viruses_array < $1
mapfile -t countries_array < $2

if [[ -z "${viruses_array[0]}" ]]
then
    echo No viruses found on $1
    exit 1
fi

if [[ -z "${countries_array[0]}" ]]
then
    echo No countries found on $2
    exit 1
fi

clen=$((${#countries_array[@]}))
vlen=$((${#viruses_array[@]}))


declare -a myarr

rm -f inputFile.txt
touch inputFile.txt

k=0
set=(a b c d e f g h i j k l m n o p q r s t u w u x y z)
if [[ ! "$4" -eq 0 ]]
then
    declare -A master_array
    citizenID=$(($RANDOM % 10000))
    for ((i = 0 ; i < $3 ; i++)); do

        make_some_doubles=$((1+$RANDOM% 20)) 
      
        if [[ "$make_some_doubles" -lt 16 ]]  #adding a chance of extra doubles by not updating the citizenID ,theoretically 5 out of 20 times "1/4" .Also it improves the speed a bit.
        then
            citizenID=$(($RANDOM % 10000))
        fi

        if [[ -z "${master_array["$citizenID"]}" ]]
        then
            Len=$((3+$RANDOM%9))
            name=''
            for ((j = 0 ; j < $Len ; j++));
            do
                name=${name}${set[$(($RANDOM % 26 ))]}
            done

            Len=$((3+$RANDOM%9))
            surname=''
            for ((j = 0 ; j < $Len ; j++));
            do
                surname=${surname}${set[$(($RANDOM % 26 ))]}
            done

            country=${countries_array[$(($RANDOM % $clen))]}
            age=$((1 + $RANDOM % 120))
            master_array["$citizenID"]="$citizenID ${name^} ${surname^} $country $age"
            virus=${viruses_array[$(($RANDOM % $vlen))]}
            if [[ $(($RANDOM % 2)) -eq 0 ]]
            then
                date="$((1 + $RANDOM % 30))-$((1 + $RANDOM % 12))-$((2000 + $RANDOM % 22))"
                infected="YES "$date
            else
                infected="NO"
            fi
            echo "${master_array["$citizenID"]} $virus $infected" >> inputFile.txt
        else
        virus=${viruses_array[$(($RANDOM % $vlen))]}
        if [[ $(($RANDOM % 2)) -eq 0 ]]
        then
            date="$((1 + $RANDOM % 30))-$((1 + $RANDOM % 12))-$((2000 + $RANDOM % 22))"
            infected="YES "$date
        else
            infected="NO"
        fi
        echo "${master_array["$citizenID"]} $virus $infected" >> inputFile.txt
        fi
    done
    exit 0
fi

i=0
declare -a idarray
idarray=($(seq 0 1 9999))
idarray=($(shuf -e "${idarray[@]}"))

for ((i = 0 ; i < $3 ; i++))
do
    citizenID="${idarray[$i]}"
    Len=$((3+$RANDOM%9))
    name=''
    for ((j = 0 ; j < $Len ; j++));
    do
        name=${name}${set[$(($RANDOM % 26 ))]}
    done

    Len=$((3+$RANDOM%9))
    surname=''
    for ((j = 0 ; j < $Len ; j++))
    do
        surname=${surname}${set[$(($RANDOM % 26 ))]}
    done

    country="${countries_array[$(($RANDOM % $clen))]}"
    age="$((1 + $RANDOM % 120))"
    virus="${viruses_array[$(($RANDOM % $vlen))]}"
    if [[ $(($RANDOM % 2)) -eq 0 ]]
    then
        date="$((1 + $RANDOM % 30))-$((1 + $RANDOM % 12))-$((2000 + $RANDOM % 22))"
        infected="YES "$date
    else
        infected="NO"
    fi
    echo "$citizenID ${name^} ${surname^} $country $age $virus $infected" >> inputFile.txt
    if  [[ "$i" -gt 9999 ]]
    then
        echo "Cant make more than 10k unique ids run with last argument > 0 to make the file with doubles and as many lines as you want"
        exit 1
    fi
done
exit 0