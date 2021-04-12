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

const std::string ArdOCL::cl_flags = "-x clc++ -cl-std=CL2.0 -O2";

ArdOCL::ArdOCL(std::vector<std::complex<double>> *data) : oCLFFT(data) {

	this->size = data->size();
	this->data_size = sizeof(double) * this->size;
	this->real = (double*) malloc(this->data_size);
	this->imag = (double*) malloc(this->data_size);
	for(size_t i = 0; i < this->size; i++) {
		std::complex<double> elem = (*data)[i];
		this->real[i] = elem.real();
		this->imag[i] = elem.imag();
	}

	std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);
	if(all_platforms.size() == 0) {
		std::cout << " No platforms found. Check OpenCL installation!\n";
		exit(1);
	}
	cl::Platform default_platform=all_platforms[0];
	std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

	std::vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	if(all_devices.size() == 0) {
		std::cout << " No devices found. Check OpenCL installation!\n";
		exit(1);
	}
	this->cl_device = all_devices[0];
	std::cout << "Using device: " << this->cl_device.getInfo<CL_DEVICE_NAME>() << std::endl;

	size_t buff_size = 0;
	this->cl_device.getInfo(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, &buff_size);
	std::cout << "Cache size: " << buff_size;
	this->cl_device.getInfo(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, &buff_size);
	std::cout << ", Constant size: " << buff_size;
	this->cl_device.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &buff_size);
	std::cout << ", Global size: " << buff_size << std::endl;

	this->cl_context = cl::Context({this->cl_device});
	cl::Program::Sources sources;
	sources.push_back({&_binary_lookup_cl_start, static_cast<cl::size_type>((&_binary_lookup_cl_end - &_binary_lookup_cl_start))});
	sources.push_back({&_binary_kernels_cl_start, static_cast<cl::size_type>((&_binary_kernels_cl_end - &_binary_kernels_cl_start))});

	this->cL_program = cl::Program(this->cl_context, sources);
	if(this->cL_program.build({this->cl_device}, cl_flags.c_str()) != CL_SUCCESS) {
		std::cout << "Error building: " << this->cL_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->cl_device) << std::endl;
		exit(1);
	}

	this->cl_buffer_r = cl::Buffer(this->cl_context, CL_MEM_READ_WRITE, this->data_size);
	this->cl_buffer_i = cl::Buffer(this->cl_context, CL_MEM_READ_WRITE, this->data_size);

	this->cl_queue = cl::CommandQueue(this->cl_context, this->cl_device);

	auto begin = std::chrono::high_resolution_clock::now();
	this->cl_queue.enqueueWriteBuffer(this->cl_buffer_r, CL_TRUE, 0, this->data_size, this->real);
	this->cl_queue.enqueueWriteBuffer(this->cl_buffer_i, CL_TRUE, 0, this->data_size, this->imag);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Copy host to device: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

//	cl::Kernel kernel_add = cl::Kernel(this->cL_program, "print_layout");
//	this->cl_queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(this->size), cl::NDRange(32));
//	this->cl_queue.finish();
}

void ArdOCL::synchronize() {
	auto begin = std::chrono::high_resolution_clock::now();
	this->cl_queue.enqueueReadBuffer(this->cl_buffer_r, CL_TRUE, 0, this->data_size, this->real);
	this->cl_queue.enqueueReadBuffer(this->cl_buffer_i, CL_TRUE, 0, this->data_size, this->imag);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Copy device to host: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

	for(size_t i = 0; i < this->size; i++) {
		(*this->data)[i].real(this->real[i]);
		(*this->data)[i].imag(this->imag[i]);
	}
}

void ArdOCL::window() {
	cl::Kernel kernel_add = cl::Kernel(this->cL_program, "window");
	kernel_add.setArg(0, this->cl_buffer_r);
	kernel_add.setArg(1, this->cl_buffer_i);
	kernel_add.setArg(2, static_cast<cl::size_type>(sizeof(size_t)), &this->size);
	this->cl_queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(this->size>>1), cl::NullRange);
	this->cl_queue.finish();
}


void ArdOCL::reverse() {
	cl::Kernel kernel_add = cl::Kernel(this->cL_program, "bit_reverse");
	kernel_add.setArg(0, this->cl_buffer_r);
	kernel_add.setArg(1, this->cl_buffer_i);
	kernel_add.setArg(2, static_cast<cl::size_type>(sizeof(size_t)), &this->size);
	this->cl_queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(1), cl::NullRange);
	this->cl_queue.finish();
}

void ArdOCL::compute() {
	auto cl_size_t = static_cast<cl::size_type>(sizeof(size_t));
	cl::Kernel kernel = cl::Kernel(this->cL_program, "fft_pow");
	kernel.setArg(0, this->cl_buffer_r);
	kernel.setArg(1, this->cl_buffer_i);

	size_t power = 0;
	while (((this->size >> power) & 1) != 1) power++;

	for(size_t i = 0; i < power; i++) {
		kernel.setArg(2, cl_size_t, &i);
		kernel.setArg(3, cl_size_t, &L[i]);
		kernel.setArg(4, cl_size_t, &L[i + 1]);
		kernel.setArg(5, cl_size_t, &C1[i]);
		kernel.setArg(6, cl_size_t, &C2[i]);
		this->cl_queue.enqueueNDRangeKernel(
			kernel, cl::NullRange,cl::NDRange(this->size/L[i + 1], L[i]),
			cl::NullRange
		);
		// std::cout << "pow: " << i << " size(" << this->size/L[i + 1] << ", " << L[i] << ")" << std::endl;
	}

	this->cl_queue.finish();
}

void ArdOCL::magnitude() {
	cl::Kernel kernel_add = cl::Kernel(this->cL_program, "magnitude");
	kernel_add.setArg(0, this->cl_buffer_r);
	kernel_add.setArg(1, this->cl_buffer_i);
	this->cl_queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(this->size), cl::NullRange);
	this->cl_queue.finish();
}
