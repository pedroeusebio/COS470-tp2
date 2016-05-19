#!/bin/bash

g++ prodcons.cpp -o pcm -std=c++11 -pthread -DPRIME -O
g++ prodcons-simple.cpp -o pc -std=c++11 -pthread -DMYSEMAPHORE -DPRIME -O
g++ prodcons-simple.cpp -o pcs -std=c++11 -pthread -DPRIME -O
g++ prodcons-c.cpp -o pcc -std=c++11 -pthread -DPRIME -O
