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

#ifndef OCLFFT_TESTS_H
#define OCLFFT_TESTS_H

#include <cstddef>
#include <cmath>
#include <cstdint>

#include "oclfft.hpp"

#define GENERATE_OUTPUT 3
#define OUTPUT_IMAGINARY 0

// These are probably defined around four times in the entire code base #oops.
static constexpr double T_ONE_PI = M_PI;
static constexpr double T_TWO_PI = 2 * T_ONE_PI;
static constexpr double T_FOUR_PI = 4 * T_ONE_PI;
static constexpr double T_SIX_PI = 6 * T_ONE_PI;

/** bitfield struct to isolate parts of double */
typedef union {
	double f;
	struct {
		uint64_t mantisa : 52;
		uint64_t exponent : 11;
		uint64_t sign : 1;
	} parts;
} double_cast;

template <class T> T sq(T x) {
	return x * x;
}

void test_fftw(double *in, double *out_r, double *out_i, size_t n);

/**
 * Generates a sinewave at the desired frequency for an supposed sample
 * rate. Please ensure sample_rate is at least double of frequency if you
 * want to detect the generated sinewave #nyquist.
 * @param n number of input samples
 * @param rate supposed rate at which input has been sampled
 */
void generate_sinewave(
	double *data, size_t n, double frequency, size_t rate);

/**
 * Alternative implementation as to provided by oclfftsup library. These
 * alternatives are more c native.
 */
void complex_to_magnitude(double *data_r, double *data_i, size_t n);

/**
 * Alternative implementation as to provided by oclfftsup library. These
 * alternatives are more c native.
 */
void window_nuttall(double *data_r, size_t n);

/**
 * Performs the bit reversal that is part of Cooley-Tukey reordered in-place
 * FFT as a separate function for verification.
 */
void bit_reversal(double *data_r, double *data_i, size_t n);

#endif //OCLFFT_TESTS_H
