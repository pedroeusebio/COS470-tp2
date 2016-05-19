#!/bin/bash

rm output-8-dumb.txt

echo "running N=10^8\n" >> output-8-dumb.txt
for K in 1 2 3 4 5 6 7 8 9 10 15 20 30
 do
    echo "running for K= $K\n" >> output-8-dumb.txt
    for T in {1..10}
    do
        echo "Times= $T" >> output-8-dumb.txt
        { /usr/bin/time -f "\t%E real,\t%U user,\t%S sys" ./spinlock-dumb 100000000 $K 1 >> output-8-dumb.txt ; } 2>> output-8-dumb.txt
        echo "\n" >> output-8-dumb.txt
    done
    echo "------------------------\n" >> output-8-dumb.txt
done