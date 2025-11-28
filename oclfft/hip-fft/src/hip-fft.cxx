/*
	oCLFFT
	Copyright (C) 2025 Corne Lukken

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

#include "hip-fft.hpp"

HipFFT::HipFFT(
    std::vector<std::complex<double>> *data, oclfft::options opts, Results *results) : oCLFFT(data
) {
    this->results = results;
	this->size = data->size();
	this->data_size = this->size * sizeof(hipfftComplex);

	hipStreamCreate(&this->stream);
	hipEventCreate(&this->start);
	hipEventCreate(&this->stop);

	hipMalloc(&this->hip_data, this->data_size);

	hipfftPlan1d(&plan, this->size, HIPFFT_C2C, 1);
	hipfftSetStream(plan, stream);
}

HipFFT::~HipFFT() {
	hipFree(hip_data);

	hipEventDestroy(start);
	hipEventDestroy(stop);

	hipStreamDestroy(stream);

	hipfftDestroy(plan);
}

void HipFFT::push() {
	std::vector<hipfftComplex> cx(this->size);
	for (size_t i = 0; i < this->size; i++) {
		cx[i].x = data->at(i).real();
		cx[i].y = data->at(i).imag();
	}

	auto begin = std::chrono::high_resolution_clock::now();
	hipMemcpy(hip_data, cx.data(), this->data_size, hipMemcpyHostToDevice);
	auto end = std::chrono::high_resolution_clock::now();
	results->copy_host_to_device.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count());
}

void HipFFT::synchronize() {
	std::vector<hipfftComplex> y(this->data_size);

	auto begin = std::chrono::high_resolution_clock::now();
	hipMemcpy(y.data(), hip_data, this->data_size, hipMemcpyDeviceToHost);
	auto end = std::chrono::high_resolution_clock::now();
	results->copy_device_to_host.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count());

	for (size_t i = 0; i < this->size; i++) {
		data->at(i).real(y[i].x);
		data->at(i).imag(y[i].y);
	}
}

void HipFFT::window() {

}

void HipFFT::compute() {
	hipEventRecord(start, stream);
	hipfftExecC2C(plan, hip_data, hip_data, HIPFFT_FORWARD);
	hipEventRecord(stop, stream);
	hipEventSynchronize(stop);
}

void HipFFT::magnitude() {

}