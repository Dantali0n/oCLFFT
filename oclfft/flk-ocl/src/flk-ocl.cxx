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

#include "flk-ocl.hpp"

const std::string FlkOCL::cl_flags = "-O2 -x clc++ -cl-std=CL2.0";

FlkOCL::FlkOCL(std::vector<std::complex<double>> *data) : oCLFFT(data) {

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
	this->cl_buffer_c = cl::Buffer(this->cl_context, CL_MEM_READ_ONLY, this->data_size);

	this->cl_queue = cl::CommandQueue(this->cl_context, this->cl_device);

	// assume sqrt(n) is integer!
	// Arrange elements into 2D array of sqrt(n)*sqrt(n).
	const size_t height = sqrt(this->size);
	// assume square
	const size_t width = height;

	// the number of non-zero containing bits in columns is log2(n)/2 as the
	// other half of the bits are divided among the rows.
	uint32_t num_bits = log2(this->size)/2;
	this->lookup_size = width*sizeof(uint32_t);
	this->lookup = (uint32_t *) malloc(lookup_size);

	// will add redundant entries were the lookup matches the swap these will
	// be handled by kernel as it is much easier to just schedule them instead
	// of complicating the computation of current row + column. The zero index
	// is never scheduled and kernels do x+1 to skip it but it is easier to have
	// in lookup.
	for(uint32_t i = 0; i < width-1; i++) {
		this->lookup[i] = reverse_bit(i, num_bits);
	}

	this->cl_buffer_l = cl::Buffer(this->cl_context, CL_MEM_READ_ONLY, this->lookup_size);

	auto begin = std::chrono::high_resolution_clock::now();
	this->cl_queue.enqueueWriteBuffer(this->cl_buffer_r, CL_TRUE, 0, this->data_size, this->real);
	this->cl_queue.enqueueWriteBuffer(this->cl_buffer_i, CL_TRUE, 0, this->data_size, this->imag);
	this->cl_queue.enqueueWriteBuffer(this->cl_buffer_l, CL_TRUE, 0, this->lookup_size, this->lookup);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Copy host to device: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

//	cl::Kernel kernel_add = cl::Kernel(this->cL_program, "print_layout");
//	this->cl_queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(this->size), cl::NDRange(32));
}

void FlkOCL::synchronize() {
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

void FlkOCL::window() {
	auto cl_size_t = static_cast<cl::size_type>(sizeof(size_t));
	cl::Kernel kernel_add = cl::Kernel(this->cL_program, "window");
	kernel_add.setArg(0, this->cl_buffer_r);
	kernel_add.setArg(1, this->cl_buffer_i);
	kernel_add.setArg(2, cl_size_t, &this->size);
	this->cl_queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(this->size>>1), cl::NullRange);
	this->cl_queue.finish();
}

void FlkOCL::reverse() {
	auto cl_size_t = static_cast<cl::size_type>(sizeof(size_t));

	// assume sqrt(n) is integer!
	// Arrange elements into 2D array of sqrt(n)*sqrt(n).
	const size_t height = sqrt(this->size);
	const size_t width = height;

	// step1 column bit-reverse by index patterns
	// number of columns to swap is half of the width -1 maximum as the first
	// column is all zero bits and always left in place.
	const size_t block_x = width-2; // columns
	size_t block_y = height; // rows

	cl::Kernel kernel_col = cl::Kernel(this->cL_program, "bit_column");
	kernel_col.setArg(0, this->cl_buffer_r);
	kernel_col.setArg(1, this->cl_buffer_i);
	kernel_col.setArg(2, this->cl_buffer_l);
	kernel_col.setArg(3, cl_size_t, &height);
	std::cout << "height: " << height << ", block_x: " << block_x << ", block_y: " << block_y << std::endl;

	// use block_x as first index as hope to optimize for column major accesses,
	// OpenCL will need to place block_y in same wavefronts. If performance
	// very bad swap x and y, don't forget to swap in kernel as well.
	this->cl_queue.enqueueNDRangeKernel(kernel_col, cl::NullRange, cl::NDRange(block_x, block_y), cl::NullRange);

	// step 2, matrix transpose
	this->cl_queue.enqueueCopyBuffer(this->cl_buffer_r, this->cl_buffer_c, 0, 0, this->data_size);
	//this->cl_queue.enqueueBarrierWithWaitList();

	cl::Kernel kernel_trans = cl::Kernel(this->cL_program, "transpose");
	kernel_trans.setArg(0, cl_size_t, &height);
	kernel_trans.setArg(1, cl_size_t, &width);
	kernel_trans.setArg(2, this->cl_buffer_c);
	kernel_trans.setArg(3, this->cl_buffer_r);
	this->cl_queue.enqueueNDRangeKernel(kernel_trans, cl::NullRange, cl::NDRange(height, width), cl::NDRange(16, 16));

	this->cl_queue.enqueueCopyBuffer(this->cl_buffer_i, this->cl_buffer_c, 0, 0, this->data_size);
	//this->cl_queue.enqueueBarrierWithWaitList();

	kernel_trans.setArg(3, this->cl_buffer_i);
	this->cl_queue.enqueueNDRangeKernel(kernel_trans, cl::NullRange, cl::NDRange(height, width), cl::NDRange(16, 16));

	// step 3, repeat column swap
	this->cl_queue.enqueueNDRangeKernel(kernel_col, cl::NullRange, cl::NDRange(block_x, block_y), cl::NullRange);

	this->cl_queue.finish();
}

void FlkOCL::compute() {
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
			kernel, cl::NullRange,cl::NDRange(L[i], this->size/L[i + 1]),
			cl::NullRange
		);
		// std::cout << "pow: " << i << " size(" << this->size/L[i + 1] << ", " << L[i] << ")" << std::endl;
	}

	this->cl_queue.finish();
}

void FlkOCL::magnitude() {
	cl::Kernel kernel_add = cl::Kernel(this->cL_program, "magnitude");
	kernel_add.setArg(0, this->cl_buffer_r);
	kernel_add.setArg(1, this->cl_buffer_i);
	this->cl_queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(this->size), cl::NullRange);
	this->cl_queue.finish();
}

/**
 * Source: https://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
 */
template<typename T>
T FlkOCL::reverse_bit(T n, size_t b) {
	assert(b <= std::numeric_limits<T>::digits);
	T rv = 0;

	for (size_t i = 0; i < b; ++i, n >>= 1) {
		rv = (rv << 1) | (n & 0x01);
	}

	return rv;
}