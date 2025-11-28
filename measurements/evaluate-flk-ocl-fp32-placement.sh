#!/bin/bash

for file in lofar-20m-x
do
    for x in 1024 65536 262144 1048576 4194304
    do
        for w in 1 2 4 8 32 64 128 256
        do
            echo "flk-ocl-fp32-placement-${w}_${x}.csv"
            ../build/oclfft/flk-ocl-fp32/flk-ocl-fp32 -w ${w} -i 30 -f ../csv/${file}.csv -s ${x} -o time >> flk-ocl-fp32/flk-ocl-fp32-5700xt-placement-${w}_${x}.csv
        done
    done
done