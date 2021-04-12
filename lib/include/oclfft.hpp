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

#ifndef OCLFFT_HPP
#define OCLFFT_HPP

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <fftw3.h>

#include <chrono>
#include <cmath>
#include <complex>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/**
 * Note on when shared_ptr is used: I only use shared_ptr to prolong the life of
 * objects that would otherwise go out of scope. If I can pass objects from a
 * higher scope down I won't use it and will use bare pointers instead.
 */

/**
 * In shared libraries some things are hard to put into a namespace,
 * everything that is notoriously so will be excluded. This includes
 * functions, templates, classes...
 */
namespace oclfft {

	// Default amount of samples if not specified
	constexpr size_t DEFAULT_SAMPLES = 512;

	// numeric precision limit of double
	typedef std::numeric_limits<double> db_lim;

	// Debug level to use throughout project
	constexpr int DEBUG = 0;

	// Enum to specifiy which type of output to generate
	enum Output {
		OUT_NONE,
		OUT_TIME,
		OUT_REAL,
		OUT_IMAGINARY
	};

	/**
	 * Program options structure
	 */
	struct options {
		/** values */
		size_t samples;
		Output output;

		/** owned / referenced counted */
		std::shared_ptr<std::string> file;
	};
}

/**
 * Abstract class all algorithms should implement
 */
class oCLFFT {
	public:
		explicit oCLFFT(std::vector<std::complex<double>> *data);
		~oCLFFT() = default;

		virtual void synchronize() = 0;

		virtual void window() = 0;
		virtual void compute() = 0;
		virtual void magnitude() = 0;
	protected:
		std::vector<std::complex<double>> *data;
};

/**
 * In-place computation of magnitudes for complex input data. The resulting
 * magnitude can be found in the real part of the data #pythagoras.
 */
void complex_to_magnitude(fftw_complex *data, size_t n);

/**
 * In-place bit reversal to test OpenCL counterparts
 */
void bit_reverse(std::vector<std::complex<double>> *data);

/**
 * In-place windowing using reference implementation
 */
void window_nuttall(std::vector<std::complex<double>> *data);

/**
 * Parse the arguments using boost::program_options and display helpful
 * messages if necessary.
 */
void parse_args(int argc, char* argv[], struct oclfft::options *options);

/**
 * Parse the provided input file and fill the vector with its results
 * Automatically detects if the input contains imaginary values.
 */
void parse_file(std::string *file, std::vector<std::complex<double>> *data, size_t limit);

/**
 *  Generates the output from the computed result and check against FFTW using
 *  the original data.
 */
void generate_output(std::vector<std::complex<double>> *result,
	std::vector<std::complex<double>> *original, oclfft::Output type);

/**
 * Generates output for graphs of python scripts, these are for internal use
 */
void generate_output(
	fftw_complex *data_ref, std::vector<std::complex<double>> *data_tar,
	size_t n, bool imaginary);
void generate_output(
	double *data_ref_r, double *data_ref_i, double *data_tar_r,
	double *data_tar_i, size_t n, bool imaginary);
void generate_output(
	double *data_ref_r, double *data_ref_i,
	std::vector<std::complex<double>> *data_tar, size_t n, bool imaginary);

#endif // OCLFFT_HPP
