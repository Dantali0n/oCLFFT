# Fast Fourier Transform Arduino Sequential
add_subdirectory(ard-seq)

# Discrete Fourier Transform Sequential
add_subdirectory(dft-seq)

# Discrete Fourier Transform Sequential 32 bit float
add_subdirectory(dft-seq-fp32)

# Fast Fourier Transform Arduino OpenMP
add_subdirectory(ard-openmp)

# Fast Fourier Transform Arduino OpenCL
add_subdirectory(ard-ocl)

# Fast Fourier Transform Parallel bit-reversal OpenCL
add_subdirectory(bit-ocl)

# Fast Fourier Transform Fast-Lookup
add_subdirectory(flk-ocl)

# Fast Fourier Transform Fast-Lookup 32 bit float
add_subdirectory(flk-ocl-fp32)

# Fast Fourier Transform using hipFFT
add_subdirectory(hip-fft)

# RMS error quantification
add_subdirectory(numerical-analysis)