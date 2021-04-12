#!/bin/bash

for file in dvb-t
do
    for x in 8 128 512 2048 8192 32768 131072 524288 2097152
    do
        echo "ard-ocl_${file}_${x}.csv"
        for _ in {1..10}
        do
            results=$(../cmake-build-debug/oclfft/ard-ocl/ard-ocl -f ../csv/${file}.csv -s ${x})
            device=$(echo "$results" | grep "to device:" | awk '{print $5}')
            window=$(echo "$results" | grep "Window:" | awk '{print $2}')
            reverse=$(echo "$results" | grep "Reverse:" | awk '{print $2}')
            fft=$(echo "$results" | grep "FFT:" | awk '{print $2}')
            magnitude=$(echo "$results" | grep "Magnitude:" | awk '{print $2}')
            host=$(echo "$results" | grep "host:" | awk '{print $5}')
            echo "$device,$window,$reverse,$fft,$magnitude,$host" >> ard-ocl/ard-ocl_${file}_${x}.csv
        done
    done
done