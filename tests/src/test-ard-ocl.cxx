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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestARDOCL

#include <boost/test/unit_test.hpp>

#include "ard-ocl.hpp"
#include "tests.hpp"

BOOST_AUTO_TEST_SUITE(Test_ARDOCL)

	BOOST_AUTO_TEST_CASE(TestARDOCLWindow) {

		size_t n = 64;
		size_t data_size = sizeof(double) * n;
		size_t rate = 50000;
		double frequency = 1665.51254;

		// Generate real sine wave into data
		double *data_r = (double *)malloc(data_size);
		double *data_i = (double *)malloc(data_size);
		generate_sinewave(data_r, n, frequency, rate);

		auto data = std::vector<std::complex<double>>();
		for(size_t i = 0; i < n; i++) {
			data_i[i] = 0;
			data.push_back(std::complex<double>(data_r[i]));
		}

		auto ocl = ArdOCL(&data);
		ocl.window();

		// Perform FFTW
		window_nuttall(data_r, n);

		ocl.synchronize();

		// Real
		for(size_t i = 0; i < n>>1; i++) {
			double_cast x;
			x.f = data_r[i];

			double_cast y;
			y.f = data[i].real();

			BOOST_CHECK(x.parts.exponent == y.parts.exponent);

			std::string x_str = std::to_string(x.parts.mantisa);
			std::string y_str = std::to_string(y.parts.mantisa);
			size_t limit = x_str.length() >= 3 ? 3 : x_str.length();
			for(size_t m = 0; m < limit; m++) {
				BOOST_CHECK(x_str.c_str()[m] == y_str.c_str()[m]);
			}
		}

		// Imaginary
		for(size_t i = 0; i < n>>1; i++) {
			double_cast x;
			x.f = data_i[i];

			double_cast y;
			y.f = data[i].imag();

			BOOST_CHECK(x.parts.exponent == y.parts.exponent);

			std::string x_str = std::to_string(x.parts.mantisa);
			std::string y_str = std::to_string(y.parts.mantisa);
			for(size_t m = 0; m < 3; m++) {
				BOOST_CHECK(x_str.c_str()[m] == y_str.c_str()[m]);
			}
		}

		#if GENERATE_OUTPUT == 1
			generate_output(data_r, data_i, &data, n, OUTPUT_IMAGINARY);
		#endif

		free(data_r);
		free(data_i);
	}

	BOOST_AUTO_TEST_CASE(TestARDOCLreversal) {

		size_t n = 64;
		size_t data_size = sizeof(double) * n;
		size_t rate = 50000;
		double frequency = 1665.51254;

		// Generate real sine wave into data
		double *data_r = (double *)malloc(data_size);
		double *data_i = (double *)malloc(data_size);
		generate_sinewave(data_r, n, frequency, rate);

		auto data = std::vector<std::complex<double>>();
		for(size_t i = 0; i < n; i++) {
			data_i[i] = 0;
			data.push_back(std::complex<double>(data_r[i]));
		}

		auto ocl = ArdOCL(&data);
		ocl.reverse();

		// Perform FFTW
		bit_reversal(data_r, data_i, n);

		ocl.synchronize();

		// Real
		for(size_t i = 0; i < n>>1; i++) {
			double_cast x;
			x.f = data_r[i];

			double_cast y;
			y.f = data[i].real();

			BOOST_CHECK(x.parts.exponent == y.parts.exponent);

			std::string x_str = std::to_string(x.parts.mantisa);
			std::string y_str = std::to_string(y.parts.mantisa);
			size_t limit = x_str.length() >= 3 ? 3 : x_str.length();
			for(size_t m = 0; m < limit; m++) {
				BOOST_CHECK(x_str.c_str()[m] == y_str.c_str()[m]);
			}
		}

		// Imaginary
		for(size_t i = 0; i < n>>1; i++) {
			double_cast x;
			x.f = data_i[i];

			double_cast y;
			y.f = data[i].imag();

			BOOST_CHECK(x.parts.exponent == y.parts.exponent);

			std::string x_str = std::to_string(x.parts.mantisa);
			std::string y_str = std::to_string(y.parts.mantisa);
			for(size_t m = 0; m < 3; m++) {
				BOOST_CHECK(x_str.c_str()[m] == y_str.c_str()[m]);
			}
		}

		#if GENERATE_OUTPUT == 2
			generate_output(data_r, data_i, &data, n, OUTPUT_IMAGINARY);
		#endif

		free(data_r);
		free(data_i);
	}

	BOOST_AUTO_TEST_CASE(TestARDOCLcompute) {

		size_t n = 64;
		size_t data_size = sizeof(double) * n;
		size_t rate = 50000;
		double frequency = 1665.51254;

		// Generate real sine wave into data
		double *data_r = (double *)malloc(data_size);
		double *data_i = (double *)malloc(data_size);
		generate_sinewave(data_r, n, frequency, rate);

		auto data = std::vector<std::complex<double>>();
		for(size_t i = 0; i < n; i++) {
			data_i[i] = 0;
			data.push_back(std::complex<double>(data_r[i]));
		}

		auto ocl = ArdOCL(&data);
		ocl.reverse();
		ocl.compute();

		// Setup FFTW data
		double *data_out_r = (double *)malloc(data_size);
		double *data_out_i = (double *)malloc(data_size);

		// Perform FFTW
		test_fftw(data_r, data_out_r, data_out_i, n);

		ocl.synchronize();

		// Real
		for(size_t i = 0; i < n>>1; i++) {
			double_cast x;
			x.f = data_out_r[i];

			double_cast y;
			y.f = data[i].real();

			BOOST_CHECK(x.parts.exponent == y.parts.exponent);

			std::string x_str = std::to_string(x.parts.mantisa);
			std::string y_str = std::to_string(y.parts.mantisa);
			size_t limit = x_str.length() >= 3 ? 3 : x_str.length();
			for(size_t m = 0; m < limit; m++) {
				BOOST_CHECK(x_str.c_str()[m] == y_str.c_str()[m]);
			}
		}

		// Imaginary
		for(size_t i = 0; i < n>>1; i++) {
			double_cast x;
			x.f = data_out_i[i];

			double_cast y;
			y.f = data[i].imag();

			BOOST_CHECK(x.parts.exponent == y.parts.exponent);

			std::string x_str = std::to_string(x.parts.mantisa);
			std::string y_str = std::to_string(y.parts.mantisa);
			for(size_t m = 0; m < 3; m++) {
				BOOST_CHECK(x_str.c_str()[m] == y_str.c_str()[m]);
			}
		}

		#if GENERATE_OUTPUT == 3
			generate_output(data_out_r, data_out_i, &data, n, OUTPUT_IMAGINARY);
		#endif

		free(data_r);
		free(data_i);

		free(data_out_r);
		free(data_out_i);
	}

BOOST_AUTO_TEST_SUITE_END()