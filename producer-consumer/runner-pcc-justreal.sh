#!/bin/bash

for n in 2, 4, 8, 16, 32
do
	echo "----- $n -----"

	echo "(1,1)"
	at=0
	for i in {1..10}
	do
		ts=$(date +%s%N) ; ./pcc $n 1 1 > garbage ; tt=$((($(date +%s%N) - $ts)/1000000)) ; at=$(($at + $tt))
	done
	echo "$at"
	
	echo "(2,2)"
	bt=0
	for i in {1..10}
	do
		ts=$(date +%s%N) ; ./pcc $n 2 2 > garbage ; tt=$((($(date +%s%N) - $ts)/1000000)) ; bt=$(($bt + $tt))
	done
	echo "$bt"

	echo "(5,5)"
	ct=0
	for i in {1..10}
	do
		ts=$(date +%s%N) ; ./pcc $n 5 5 > garbage ; tt=$((($(date +%s%N) - $ts)/1000000)) ; ct=$(($ct + $tt))
	done
	echo "$ct"

	echo "(10,10)"
	dt=0
	for i in {1..10}
	do
		ts=$(date +%s%N) ; ./pcc $n 10 10 > garbage ; tt=$((($(date +%s%N) - $ts)/1000000)) ; dt=$(($dt + $tt))
	done
	echo "$dt"

	echo "(2,10)"
	et=0
	for i in {1..10}
	do
		ts=$(date +%s%N) ; ./pcc $n 2 10 > garbage ; tt=$((($(date +%s%N) - $ts)/1000000)) ; et=$(($et + $tt))
	done
	echo "$et"

	echo "(10,2)"
	ft=0
	for i in {1..10}
	do
		ts=$(date +%s%N) ; ./pcc $n 10 2 > garbage ; tt=$((($(date +%s%N) - $ts)/1000000)) ; ft=$(($ft + $tt))
	done
	echo "$ft"
done