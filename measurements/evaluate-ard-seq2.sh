#!/bin/bash

for file in dvb-t
do
    for x in 8 16 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152
    do
        echo "ard-seq-rev_${file}_${x}.csv"
        for _ in {1..30}
        do
            results=$(../cmake-build-debug/oclfft/ard-seq/ard-seq -f ../csv/${file}.csv -s ${x})
            reverse=$(echo "$results" | grep "Bit reverse:" | awk '{print $3}')
            echo "$reverse" >> ard-seq/ard-seq-rev_${file}_${x}.csv
        done
    done
done