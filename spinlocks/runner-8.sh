#!/bin/bash

rm output-8.txt

echo "running N=10^8\n" >> output-8.txt
for K in 1 2 3 4 5 6 7 8 9 10 15 20 30
 do
    echo "running for K= $K\n" >> output-8.txt
    for T in {1..10}
    do
        echo "Times= $T" >> output-8.txt
        { /usr/bin/time -f "\t%E real,\t%U user,\t%S sys" ./spinlock 100000000 $K 1 >> output-8.txt ; } 2>> output-8.txt
        echo "\n" >> output-8.txt
    done
    echo "------------------------\n" >> output-8.txt
done