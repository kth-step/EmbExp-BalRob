#!/usr/bin/env bash

num_iter=2

for (( i=1; i<=${num_iter}; i++ ))
do
   echo "Run bundle #${i} of ${num_iter}"
   ./benchmark.py
done

