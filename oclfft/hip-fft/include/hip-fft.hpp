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

#ifndef hipfft_h
#define hipfft_h

#include <cstdint>
#include <cstdio>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

#include <hip/hip_runtime.h>
#include "hip/hip_vector_types.h"
#include <hipfft/hipfft.h>

#include "oclfft.hpp"
#include "results.hpp"

class HipFFT : public oCLFFT {
public:
	HipFFT(std::vector<std::complex<double>> *data, oclfft::options opts, Results *results); // : oCLFFT(data)
	virtual ~HipFFT();
    void push() override;
	void synchronize() override;
	void window() override;
	void compute() override;
	void magnitude() override;
protected:
    Results *results;
	size_t size;
	size_t data_size;
	hipfftComplex *hip_data;

	hipfftHandle plan;
	hipStream_t stream;
	hipEvent_t start;
	hipEvent_t stop;

	static const std::string cl_flags;
};

#endif // hipfft_h