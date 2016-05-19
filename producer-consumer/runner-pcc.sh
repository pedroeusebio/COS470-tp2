#!/bin/bash

echo "Results" > results.txt

for n in 2, 4, 8, 16, 32
do
	echo "Testing for buffer size $n ..."
	echo "N = $n" > results.txt
	echo "-------------------- N = $n --------------------"
	echo
	echo "---------- (1,1) ----------"
	for i in {1..10}
	do
		echo "$i (1, 1)"
		time ./pcc $n 1 1 > results.txt
		echo "----------"
	done
	echo
	echo "---------- (2,2) ----------"
	for i in {1..10}
	do
		echo "$i (2, 2)"
		time ./pcc $n 2 2 > results.txt
		echo "----------"
	done
	echo
	echo "---------- (5,5) ----------"
	for i in {1..10}
	do
		echo "$i (5, 5)"
		time ./pcc $n 5 5 > results.txt
		echo "----------"
	done
	echo
	echo "---------- (10,10) ----------"
	for i in {1..10}
	do
		echo "$i (10, 10)"
		time ./pcc $n 10 10 > results.txt
		echo "----------"
	done
	echo
	echo "---------- (2,10) ----------"
	for i in {1..10}
	do
		echo "$i (2, 10)"
		time ./pcc $n 2 10 > results.txt
		echo "----------"
	done
	echo
	echo "---------- (10,2) ----------"
	for i in {1..10}
	do
		echo "$i (10, 2)"
		time ./pcc $n 10 2 > results.txt
		echo "----------"
	done
	echo
	echo "Done"
done