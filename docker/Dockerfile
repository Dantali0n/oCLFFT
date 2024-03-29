FROM debian:bookworm

RUN --mount=type=cache,target=/var/cache/apt apt-get update
RUN --mount=type=cache,target=/var/cache/apt apt-get install -y pkg-config \
    cmake make gcc g++ libboost-all-dev ocl-icd-opencl-dev opencl-headers \
    opencl-clhpp-headers opencl-c-headers libfftw3-dev \
    libmpfr-dev libmpfrc++-dev python3 virtualenv \
    doxygen texinfo texlive texlive-science texlive-publishers texlive-bibtex-extra
