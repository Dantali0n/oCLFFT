#!/bin/bash

for file in lofar-20m-x
do
    for x in 16384 65536 262144 1048576
    do
        echo "flk-ocl-5700xt_${x}.csv"
        for _ in {1..10}
        do
            ../build/oclfft/flk-ocl/flk-ocl -w 256 -i 30 -f ../csv/${file}.csv -s ${x} -o time >> flk-ocl/flk-ocl-5700xt-placement_${x}.csv
        done
    done
done