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

const std::string FlkOCL32::cl_flags = "-O2 -x clc++ -cl-std=CL2.0";

FlkOCL32::FlkOCL32(
    std::vector<std::complex<double>> *data, oclfft::options opts, Results *results) : oCLFFT(data
) {
    this->results = results;
	this->size = data->size();
	this->data_size = sizeof(float) * this->size;
    this->wavefront_size = opts.wavefront;
	this->real = (float*) malloc(this->data_size);
	this->imag = (float*) malloc(this->data_size);

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

    auto begin = std::chrono::high_resolution_clock::now();
	this->cl_context = cl::Context({this->cl_device});
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Create context: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

	cl::Program::Sources sources;
	sources.push_back({&_binary_lookup_cl_start, static_cast<cl::size_type>((&_binary_lookup_cl_end - &_binary_lookup_cl_start))});
	sources.push_back({&_binary_kernels_cl_start, static_cast<cl::size_type>((&_binary_kernels_cl_end - &_binary_kernels_cl_start))});

    begin = std::chrono::high_resolution_clock::now();
	this->cL_program = cl::Program(this->cl_context, sources);
	if(this->cL_program.build({this->cl_device}, cl_flags.c_str()) != CL_SUCCESS) {
		std::cout << "Error building: " << this->cL_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->cl_device) << std::endl;
		exit(1);
	}
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Compile sources: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
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
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Create buffers: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << "" << std::endl;

    this->cl_queue.enqueueWriteBuffer(this->cl_buffer_l, CL_TRUE, 0, this->lookup_size, this->lookup);

    cl::Kernel kernel_add = cl::Kernel(this->cL_program, "dummy_operation");
    if(this->cl_queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(this->size>>1), cl::NDRange(this->wavefront_size)) != CL_SUCCESS) {
        std::cerr << "Failed to enqueue dummy_operation" << std::endl;
    }
    this->cl_queue.finish();
}

void FlkOCL32::push() {
    for(size_t i = 0; i < this->size; i++) {
        std::complex<double> elem = (*data)[i];
        this->real[i] = (float)elem.real();
        this->imag[i] = (float)elem.imag();
    }

    auto begin = std::chrono::high_resolution_clock::now();
    this->cl_queue.enqueueWriteBuffer(this->cl_buffer_r, CL_TRUE, 0, this->data_size, this->real);
    this->cl_queue.enqueueWriteBuffer(this->cl_buffer_i, CL_TRUE, 0, this->data_size, this->imag);
    auto end = std::chrono::high_resolution_clock::now();
    results->copy_host_to_device.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count());
}

void FlkOCL32::synchronize() {
	auto begin = std::chrono::high_resolution_clock::now();
	this->cl_queue.enqueueReadBuffer(this->cl_buffer_r, CL_TRUE, 0, this->data_size, this->real);
	this->cl_queue.enqueueReadBuffer(this->cl_buffer_i, CL_TRUE, 0, this->data_size, this->imag);
	auto end = std::chrono::high_resolution_clock::now();
    results->copy_device_to_host.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count());

	for(size_t i = 0; i < this->size; i++) {
		(*this->data)[i].real((double)this->real[i]);
		(*this->data)[i].imag((double)this->imag[i]);
	}
}

void FlkOCL32::window() {
	auto cl_size_t = static_cast<cl::size_type>(sizeof(size_t));
	cl::Kernel kernel_add = cl::Kernel(this->cL_program, "window");
	kernel_add.setArg(0, this->cl_buffer_r);
	kernel_add.setArg(1, this->cl_buffer_i);
	kernel_add.setArg(2, cl_size_t, &this->size);
    kernel_add.setArg(3, cl_size_t, &this->wavefront_size);

	if(this->cl_queue.enqueueNDRangeKernel(kernel_add, cl::NullRange, cl::NDRange(this->size>>1), cl::NDRange(this->wavefront_size)) != CL_SUCCESS) {
        std::cerr << "Failed to enqueue window" << std::endl;
    }
	this->cl_queue.finish();
}

void FlkOCL32::reverse() {
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
    kernel_col.setArg(4, cl_size_t, &this->wavefront_size);

//    std::cout << "height: " << height << ", block_x: " << block_x << ", block_y: " << block_y << std::endl;

	// use block_x as first index as hope to optimize for column major accesses,
	// OpenCL will need to place block_y in same wavefronts. If performance
	// very bad swap x and y, don't forget to swap in kernel as well.
    auto begin = std::chrono::high_resolution_clock::now();
	if(this->cl_queue.enqueueNDRangeKernel(kernel_col, cl::NullRange, cl::NDRange(block_x, block_y), cl::NDRange(2, this->wavefront_size)) != CL_SUCCESS) {
        std::cerr << "Failed to enqueue bit_column" << std::endl;
    }
//    this->cl_queue.finish();
    auto end = std::chrono::high_resolution_clock::now();
//    std::cout << "Bit column: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << std::endl;

	// step 2, matrix transpose
    if(this->cl_queue.enqueueCopyBuffer(this->cl_buffer_r, this->cl_buffer_c, 0, 0, this->data_size) != CL_SUCCESS) {
        std::cerr << "Failed to enqueue copy buffer R to C" << std::endl;
    }

	cl::Kernel kernel_trans = cl::Kernel(this->cL_program, "transpose");
	kernel_trans.setArg(0, cl_size_t, &height);
	kernel_trans.setArg(1, cl_size_t, &width);
	kernel_trans.setArg(2, this->cl_buffer_c);
	kernel_trans.setArg(3, this->cl_buffer_r);

    begin = std::chrono::high_resolution_clock::now();
    if(this->cl_queue.enqueueNDRangeKernel(kernel_trans, cl::NullRange, cl::NDRange(height, width), cl::NDRange(16, 16)) != CL_SUCCESS) {
        std::cerr << "Failed to enqueue transpose R" << std::endl;
    }
//    this->cl_queue.finish();
    end = std::chrono::high_resolution_clock::now();
//    std::cout << "transpose R: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << std::endl;

    if(this->cl_queue.enqueueCopyBuffer(this->cl_buffer_i, this->cl_buffer_c, 0, 0, this->data_size) != CL_SUCCESS) {
        std::cerr << "Failed to enqueue copy buffer I to C" << std::endl;
    }

	kernel_trans.setArg(3, this->cl_buffer_i);
    begin = std::chrono::high_resolution_clock::now();
	if(this->cl_queue.enqueueNDRangeKernel(kernel_trans, cl::NullRange, cl::NDRange(height, width), cl::NDRange(16, 16)) != CL_SUCCESS) {
        std::cerr << "Failed to enqueue transpose I" << std::endl;
    }
//    this->cl_queue.finish();
    end = std::chrono::high_resolution_clock::now();
//    std::cout << "transpose I: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << std::endl;

	// step 3, repeat column swap
    begin = std::chrono::high_resolution_clock::now();
	if(this->cl_queue.enqueueNDRangeKernel(kernel_col, cl::NullRange, cl::NDRange(block_x, block_y), cl::NDRange(2, this->wavefront_size))!= CL_SUCCESS) {
        std::cerr << "Failed to enqueue bit_column" << std::endl;
    }
//    this->cl_queue.finish();
    end = std::chrono::high_resolution_clock::now();
//    std::cout << "Bit column: " << std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count() << std::endl;

    if(this->cl_queue.finish() != CL_SUCCESS) {
        std::cerr << "Failed to finish bit reverse queue" << std::endl;
    }
}

void FlkOCL32::compute() {
	auto cl_size_t = static_cast<cl::size_type>(sizeof(size_t));
	cl::Kernel kernel = cl::Kernel(this->cL_program, "fft_pow");
	kernel.setArg(0, this->cl_buffer_r);
	kernel.setArg(1, this->cl_buffer_i);

	size_t power = 0;
	while (((this->size >> power) & 1) != 1) power++;

	for(size_t i = 0; i < power; i++) {
        auto local_x = std::min(L[i], (size_t)64);
        auto local_y = 64 / local_x;

		kernel.setArg(2, cl_size_t, &i);
		kernel.setArg(3, cl_size_t, &L[i]);
		kernel.setArg(4, cl_size_t, &L[i + 1]);
		kernel.setArg(5, sizeof(float), &C1[i]);
		kernel.setArg(6, sizeof(float), &C2[i]);
        kernel.setArg(7, cl_size_t, &local_x);
        kernel.setArg(8, cl_size_t, &local_y);

//        std::cout << "pow:[" << i << "][" << L[i] << "," << this->size/L[i + 1] << "]"
//                  << "[" << local_x << "," << local_y << "]" << std::endl;

        if(this->cl_queue.enqueueNDRangeKernel(
			kernel, cl::NullRange,cl::NDRange(L[i], this->size/L[i + 1]),
			cl::NDRange(local_x, local_y)
		)) {
            std::cerr << "Failed to enqueue fft_pow[" << i << "]" << std::endl;
        }

//        this->cl_queue.finish();
	}

	this->cl_queue.finish();
}

void FlkOCL32::magnitude() {
    auto cl_size_t = static_cast<cl::size_type>(sizeof(size_t));
	cl::Kernel kernel_add = cl::Kernel(this->cL_program, "magnitude");
	kernel_add.setArg(0, this->cl_buffer_r);
	kernel_add.setArg(1, this->cl_buffer_i);
    kernel_add.setArg(2, cl_size_t, &this->wavefront_size);
    if(this->cl_queue.enqueueNDRangeKernel(
        kernel_add, cl::NullRange, cl::NDRange(this->size), cl::NDRange(this->wavefront_size)) != CL_SUCCESS
    ) {
        std::cerr << "Failed to enqueue magnitude" << std::endl;
    }
	this->cl_queue.finish();
}

/**
 * Source: https://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
 */
template<typename T>
T FlkOCL32::reverse_bit(T n, size_t b) {
	assert(b <= std::numeric_limits<T>::digits);
	T rv = 0;

	for (size_t i = 0; i < b; ++i, n >>= 1) {
		rv = (rv << 1) | (n & 0x01);
	}

	return rv;
}