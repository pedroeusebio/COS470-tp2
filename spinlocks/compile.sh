#!/bin/bash

echo "Starting running script" >> output4.txt
echo "------------------------\n" >> output4.txt
#

for N in 10000000000
do
    echo "running N=$N\n" >> output4.txt
    for K in 1 2 3 4 5 6 7 8 9 10 15 20 30
     do
        echo "running for K= $K\n" >> output4.txt
        for T in 1 2 3 4 5 6 7 8 9 10
        do
            echo "Times= $T" >> output4.txt
            { /usr/bin/time -f "\t%E real,\t%U user,\t%S sys" ./main $N $K 1 >> output4.txt ; } 2>> output4.txt
            echo "\n" >> output4.txt
        done
        echo "------------------------\n" >> output4.txt
    done
done
