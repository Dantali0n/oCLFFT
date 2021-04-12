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

#include "ard-ocl.hpp"

int main(int argc, char* argv[]) {
	oclfft::options opts{};
	parse_args(argc, argv, &opts);

	auto data = std::vector<std::complex<double>>();

	parse_file(opts.file.get(), &data, opts.samples);
	auto copy = data;

	std::cout << "Samples: " << data.size() << std::endl;

	auto aocl = ArdOCL(&data);

	auto begin = std::chrono::high_resolution_clock::now();
	aocl.window();
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Window: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

	begin = std::chrono::high_resolution_clock::now();
	aocl.reverse();
	end = std::chrono::high_resolution_clock::now();
	std::cout << "Reverse: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

	begin = std::chrono::high_resolution_clock::now();
	aocl.compute();
	end = std::chrono::high_resolution_clock::now();
	std::cout << "FFT: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

	begin = std::chrono::high_resolution_clock::now();
	aocl.magnitude();
	end = std::chrono::high_resolution_clock::now();
	std::cout << "Magnitude: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

	aocl.synchronize();

	if(opts.output != oclfft::OUT_NONE)
		generate_output(&data, &copy, opts.output);
}