#!/bin/bash

for file in dvb-t
do
    for x in 16 64 256 1024 4096 16384 65536 262144 1048576
    do
        echo "bit-ocl_${file}_${x}.csv"
        for _ in {1..10}
        do
            results=$(../cmake-build-debug/oclfft/bit-ocl/bit-ocl -f ../csv/${file}.csv -s ${x})
            device=$(echo "$results" | grep "to device:" | awk '{print $5}')
            window=$(echo "$results" | grep "Window:" | awk '{print $2}')
            reverse=$(echo "$results" | grep "Reverse:" | awk '{print $2}')
            fft=$(echo "$results" | grep "FFT:" | awk '{print $2}')
            magnitude=$(echo "$results" | grep "Magnitude:" | awk '{print $2}')
            host=$(echo "$results" | grep "host:" | awk '{print $5}')
            echo "$device,$window,$reverse,$fft,$magnitude,$host" >> bit-ocl/bit-ocl_${file}_${x}.csv
        done
    done
done