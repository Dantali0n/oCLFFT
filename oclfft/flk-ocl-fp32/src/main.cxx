/*
	oCLFFT
	Copyright (C) 2021 Corne Lukken

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "flk-ocl32.hpp"
#include "results.hpp"

bool isPowerOfTwo(int n) {
    if (n == 0)
        return false;

    return (ceil(log2(n)) == floor(log2(n)));
}

int main(int argc, char* argv[]) {
    auto data = std::vector<std::complex<double>>();
    auto results = Results();
    oclfft::options opts{};

	parse_args(argc, argv, &opts);
    parse_file(opts.file.get(), &data, opts.samples);
    auto copy = data;

    if(!isPowerOfTwo(opts.wavefront) || !isPowerOfTwo(data.size())) {
        std::cerr << "Wavefront and samples must be 1 or power of 2" << std::endl;
        exit(1);
    }

    if(opts.wavefront > 256) {
        std::cerr << "Wavefront can be maximum of 128" << std::endl;
        exit(1);
    }

    if(opts.wavefront*opts.wavefront > data.size()) {
        std::cerr << "Wavefront to large for workload, minimal: " << opts.wavefront*opts.wavefront << std::endl;
        exit(1);
    }

    std::cout << "Samples: " << data.size() << std::endl;
    std::cout << "Wavefront: " << opts.wavefront << std::endl;
    std::cout << "Iterations: " << opts.iterations << std::endl;

    auto focl = FlkOCL32(&data, opts, &results);
    for(size_t i = 0; i < opts.iterations; i++) {

        data.clear();
        parse_file(opts.file.get(), &data, opts.samples);

        focl.push();

        auto begin = std::chrono::high_resolution_clock::now();
        focl.window();
        auto end = std::chrono::high_resolution_clock::now();
        results.window.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count());

        begin = std::chrono::high_resolution_clock::now();
        focl.reverse();
        end = std::chrono::high_resolution_clock::now();
        results.reverse.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count());

        begin = std::chrono::high_resolution_clock::now();
        focl.compute();
        end = std::chrono::high_resolution_clock::now();
        results.fft.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count());

        begin = std::chrono::high_resolution_clock::now();
        focl.magnitude();
        end = std::chrono::high_resolution_clock::now();
        results.magnitude.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count());

        focl.synchronize();
    }

	if(opts.output != oclfft::OUT_NONE)
        generate_output(&data, &copy, &results, opts.output);
}