/*

	FFT libray
	Copyright (C) 2010 Didier Longueville
	Copyright (C) 2014 Enrique Condes
 	Copyright (C) 2020 Corne Lukken

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
#include "ard-omp.hpp"

int main(int argc, char* argv[]) {
	oclfft::options opts{};
	parse_args(argc, argv, &opts);

	auto data = std::vector<std::complex<double>>();

	parse_file(opts.file.get(), &data, opts.samples);

	double *real = (double*)malloc(data.size() * sizeof(double));
	double *imagine = (double*)malloc(data.size() * sizeof(double));

	size_t size = data.size();

	for(size_t i = 0; i < size; i++) {
		real[i] = data[i].real();
		imagine[i] = data[i].imag();
	}

	std::cout << "Samples: " << data.size() << std::endl;

	arduinoFFT *fft = new arduinoFFT(real, imagine, size, 50000);

	auto begin = std::chrono::high_resolution_clock::now();
	fft->Windowing(FFT_WIN_TYP_NUTTALL, FFT_FORWARD);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Window: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

	begin = std::chrono::high_resolution_clock::now();
	fft->Compute(FFT_FORWARD);
	end = std::chrono::high_resolution_clock::now();
	std::cout << "FFT: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

	begin = std::chrono::high_resolution_clock::now();
	fft->ComplexToMagnitude();
	end = std::chrono::high_resolution_clock::now();
	std::cout << "Magnitude: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

	auto copy = std::vector<std::complex<double>>();
	for(size_t i = 0; i < size; i++) {
		copy.push_back(std::complex<double>(real[i], imagine[i]));
	}

	if(opts.output != oclfft::OUT_NONE)
		generate_output(&copy, &data, opts.output);

	free(real);
	free(imagine);
}