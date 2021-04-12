# oCLFFT
### openCL Fast Fourier Transform

Project for University of Amsterdam (UvA) performance engineering course,
boilerplate CMake files are from
[a previous project](https://github.com/hexoxide/O2-Balancer2)

### Directory structure

* cmake - small cmake snippets to enable various features
* csv - converted wavefiles that can be used by applications
* final - LaTeX final presentation given at the end of the course
* lib - support library with functions and definitions
* measurements - performance evaluation scripts as well as measured values
* midterm - LaTeX files for the presentation at the first half of the course
* playground - small toy examples or other experiments
* [python](python/README.md) - python scripts to aid in visualization or measurements
* proposal - LaTeX source for project proposal
* report - LaTeX source for project report
* src - project source files
* tests - unit tests and possibly integration tests
* wav - Original wavefile formats used as input data 

### Implementations

| Executable | Hardware Target   | Base Algorithm | Properties                    |
|------------|-------------------|----------------|-------------------------------|
| ard-seq    | CPU Single Thread | Cooley-Tukey   | Bit-reversal in-place radix-2 |
| dft-seq    | CPU Single Thread | DFT            |                               |
| ard-omp    | CPU OpenMP        | Cooley-Tukey   | Bit-reversal in-place radix-2 |
| ard-ocl    | GPU               | Cooley-Tukey   | Slow bit-reversal, fft lookup |
| bit-ocl    | GPU               | Cooley-Tukey   | 2D bit-reversal, fft lookup   |
| flk-ocl    | GPU               | Cooley-Tukey   | 2D bit-reversal, fast lookup  |

#### Dependencies

* cmake 3.0 or higher
* cmake 3.9 or higher (ard-omp)
* doxygen (documentation)
* boost 1.32 or higher
* boost 1.53 or higher (unit tests)
* python 3.x (python)
* virtualenv (python)

#### Setup

Python environment:

```bash
virtualenv -p python3 python/e
cd python
source bin/activate
pip install -r requirements.txt
```

Generating lookup tables:

```bash
mkdir build
cd build
cmake ..
make play-lookup
./playground/play-lookup > ../oclfft/ard-ocl/src/lookup.cl
./playground/play-lookup > ../oclfft/bit-ocl/src/lookup.cl
./playground/play-lookup > ../oclfft/flk-ocl/src/lookup.cl
```

#### Licensing

All project files are licensed under GPLv3 unless otherwise stated by the header
of the source files. Most files will be authored under Corne Lukken but some
source files are from third-party sources, please check the license headers to
verify the author and license.

A few examples of files licensed under different authors:

* Beamer templates by Jerome Belleman.
* ard-seq implementation by Didier Longueville & Enrique Condes.

#### References

* [Emebedding source files into binaries](https://www.linuxjournal.com/content/embedding-file-executable-aka-hello-world-version-5967)
* [OpenCL, SyCL and SPIR-V progress 2016](https://www.youtube.com/watch?v=TYp1d6yzHUQ)

#### Snippets

```bash
../cmake-build-debug/oclfft/bit-ocl/bit-ocl -f ../csv/ads-b.csv -s 262144  -o real | python evaluate-error.py
```

```bash
 /opt/rocm/bin/rocprof --obj-tracking on --hsa-trace binary
```