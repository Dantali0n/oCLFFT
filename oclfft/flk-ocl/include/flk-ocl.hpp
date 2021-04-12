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

#ifndef flkocl_h
#define flkocl_h

#include <cstdint>
#include <cstdio>
#include <cmath>

#include <chrono>

#define CL_HPP_TARGET_OPENCL_VERSION 200

#include <CL/cl2.hpp>

/** Because writing multi-line strings as code is insanity */
/*https://www.linuxjournal.com/content/embedding-file-executable-aka-hello-world-version-5967 */
/* here "kernels_cl" comes from the filename kernels.cl */
extern "C" char _binary_kernels_cl_start;
extern "C" char _binary_kernels_cl_end;

extern "C" char _binary_lookup_cl_start;
extern "C" char _binary_lookup_cl_end;

#include "oclfft.hpp"

static constexpr size_t L[] = {
	1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048,
	4096, 8192, 16384, 32768, 65536, 131072, 262144,
	524288, 1048576, 2097152, 4194304, 8388608, 16777216,
	33554432, 67108864, 134217728, 268435456, 536870912,
	1073741824, 2147483648, 4294967296
};

static constexpr double C1[] = {
	-1.0, 0.0000000000, 0.707106781186548, 0.923879532511287, 0.98078528040323,
	0.995184726672197, 0.998795456205172, 0.999698818696204, 0.999924701839145,
	0.999981175282601, 0.999995293809576, 0.999998823451702, 0.999999705862882,
	0.999999926465718, 0.999999981616429, 0.999999995404107, 0.999999998851027,
	0.999999999712757, 0.999999999928189, 0.999999999982047, 0.999999999995512,
	0.999999999998878, 0.999999999999719, 0.99999999999993, 0.999999999999982,
	0.999999999999996, 0.999999999999999, 1.0000000000, 1.0000000000,
	1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000
};

static constexpr double C2[] = {
	0.0, -1.0000000000, -0.707106781186548, -0.38268343236509, -0.195090322016128,
	-0.0980171403295606, -0.0490676743274178, -0.0245412285229122, -0.0122715382857193,
	-0.00613588464915602, -0.00306795676296871, -0.00153398018628156, -0.000766990318752704,
	-0.000383495187566529, -0.000191747597257278, -0.0000958737992800168, -0.0000479368994952578,
	-0.0000239684494581277, -0.0000119842258870685, -0.00000599211525954283, -0.00000299605994577867,
	-0.00000149802997288934, -0.000000749033514266433, -0.000000374535284267569, -0.000000187304691820987,
	-0.0000000936523459104935, -0.0000000471216091538724, -0.0000000235608045769362, -0.0000000129047841397589,
	-0.00000000745058059692383, -0.0000000000, -0.0000000000, -0.0000000000
};

class FlkOCL : public oCLFFT {
public:
	FlkOCL(std::vector<std::complex<double>> *data); // : oCLFFT(data)
	void synchronize() override;
	void window() override;
	void compute() override;
	void magnitude() override;

	void reverse();
protected:

	/**
	 * Find the bit-reverse of a value for type T up to b bits.
	 */
	template <typename T>
	T reverse_bit(T n, size_t b = sizeof(T) * CHAR_BIT);

	double *real;
	double *imag;
	uint32_t *lookup;

	size_t size;
	size_t data_size;
	size_t lookup_size;

	cl::Device cl_device;
	cl::Context cl_context;
	cl::Program cL_program;
	cl::Buffer cl_buffer_r;
	cl::Buffer cl_buffer_i;
	cl::Buffer cl_buffer_c;
	cl::Buffer cl_buffer_l;
	cl::CommandQueue cl_queue;

	static const std::string cl_flags;
};

#endif // flkocl_h