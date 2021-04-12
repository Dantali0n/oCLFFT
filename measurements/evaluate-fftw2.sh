#!/bin/bash

for file in xpa1 xpa2 jorn acars flex ads-b dvb-t
do
    for x in 16 64 256 1024 4096 16384 65536 262144 1048576
    do
        echo "fftw_${file}_${x}.csv"
        for _ in {1..30}
        do
            results=$(../cmake-build-debug/oclfft/ard-seq/ard-seq -f ../csv/${file}.csv -s ${x})
            window=$(echo "$results" | grep "FFTW window:" | awk '{print $3}')
            fft=$(echo "$results" | grep "FFTW fft:" | awk '{print $3}')
            magnitude=$(echo "$results" | grep "FFTW magnitude:" | awk '{print $3}')
            echo "$window,$fft,$magnitude" >> fftw/fftw_${file}_${x}.csv
        done
    done
done