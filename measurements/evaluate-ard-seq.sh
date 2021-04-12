#!/bin/bash

for file in xpa1 xpa2 jorn acars flex ads-b dvb-t
do
    for x in 8 128 512 2048 8192 32768 131072 524288 2097152
    do
        echo "ard-seq_${file}_${x}.csv"
        for _ in {1..30}
        do
            results=$(../cmake-build-debug/oclfft/ard-seq/ard-seq -f ../csv/${file}.csv -s ${x})
            window=$(echo "$results" | grep "Window:" | awk '{print $2}')
            fft=$(echo "$results" | grep "FFT:" | awk '{print $2}')
            magnitude=$(echo "$results" | grep "Magnitude:" | awk '{print $2}')
            echo "$window,$fft,$magnitude" >> ard-seq/ard-seq_${file}_${x}.csv
        done
    done
done