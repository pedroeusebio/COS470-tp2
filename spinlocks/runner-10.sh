#!/bin/bash

rm output-10.txt

echo "running N=10^10\n" >> output-10.txt
for K in 1 2 3 4 5 6 7 8 9 10 15 20 30
 do
    echo "running for K= $K\n" >> output-10.txt
    for T in {1..10}
    do
        echo "Times= $T" >> output-10.txt
        { /usr/bin/time -f "\t%E real,\t%U user,\t%S sys" ./spinlock 10000000000 $K 1 >> output-10.txt ; } 2>> output-10.txt
        echo "\n" >> output-10.txt
    done
    echo "------------------------\n" >> output-10.txt
done