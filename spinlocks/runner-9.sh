#!/bin/bash

rm output-9.txt

echo "running N=10^9\n" >> output-9.txt
for K in 1 2 3 4 5 6 7 8 9 10 15 20 30
 do
    echo "running for K= $K\n" >> output-9.txt
    for T in {1..10}
    do
        echo "Times= $T" >> output-9.txt
        { /usr/bin/time -f "\t%E real,\t%U user,\t%S sys" ./spinlock 1000000000 $K 1 >> output-9.txt ; } 2>> output-9.txt
        echo "\n" >> output-9.txt
    done
    echo "------------------------\n" >> output-9.txt
done