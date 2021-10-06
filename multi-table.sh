#!/bin/bash
num1=$1
num2=$2
validation="^[1-9]+$"

if ! [[ $num1 =~ $validation ]] || ! [[ $num2 =~ $validation ]]
then
	echo "Your input is wrong. Please set input by integer and bigger than 0."
	exit 1
fi

for i in $(seq 1 $num1)
do
	for j in $(seq 1 $num2)
	do
		printf $i*$j=`expr $i \* $j`" "
	done
	echo
done
exit 0
