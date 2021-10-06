#!/bin/sh
num1=$1
num2=$2

for i in $(seq 1 $num1)
do
	for j in $(seq 1 $num2)
	do
		printf $i*$j=`expr $i \* $j`" "
	done
	echo
done
exit 0
