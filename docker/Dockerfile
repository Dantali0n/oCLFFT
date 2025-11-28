FROM ubuntu:24.04

# This Docker image builds the dependencies for the application.
# It lives on the head of its dependencies.

# Install all build-time dependencies
RUN --mount=type=cache,target=/var/cache/apt \
    export DEBIAN_FRONTEND=noninteractive && \
	apt-get update && \
	apt-get install -y \
    abi-compliance-checker \
    abi-dumper \
    build-essential \
    clang-format \
    clang-tidy \
    cmake \
    cmake-format \
    curl \
    doxygen \
    texinfo \
    texlive-full \
    gcovr \
    git \
    graphviz \
    libboost-dev \
    libboost-test-dev \
    libboost-program-options-dev libboost-timer-dev \
    libfftw3-dev \
    libmpfr-dev libmpfrc++-dev \
    libsfml-dev \
    ninja-build \
    wget \
    rsync \
    libxrandr-dev libxcursor-dev libxi-dev libudev-dev libfreetype-dev \
    libflac-dev libvorbis-dev libgl1-mesa-dev libegl1-mesa-dev libfreetype-dev \
    nvidia-cuda-toolkit \
    openmpi-common openmpi-bin libopenmpi-dev \
    pre-commit python3-breathe python3-setuptools python3-wheel && \
	rm -rf /var/lib/apt/lists/*

RUN wget https://repo.radeon.com/amdgpu-install/7.0.1/ubuntu/jammy/amdgpu-install_7.0.1.70001-1_all.deb && \
   apt install ./amdgpu-install_7.0.1.70001-1_all.deb && \
   apt update && \
   apt install -y rocm hipfft libhipfft-dev
