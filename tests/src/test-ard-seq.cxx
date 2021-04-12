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
#define BOOST_TEST_MODULE TestARDSeq

#include <boost/test/unit_test.hpp>

#include "ard-omp.hpp"
#include "tests.hpp"

BOOST_AUTO_TEST_SUITE(Test_ARDSeq)

	BOOST_AUTO_TEST_CASE(TestARDSeqBare) {

		size_t n = 64;
		size_t data_size = sizeof(double) * n;
		size_t rate = 50000;
		double frequency = 1665.51254;

		// Generate real sine wave into data
		double *data_r = (double *)malloc(data_size);
		double *data_i = (double *)malloc(data_size);
		generate_sinewave(data_r, n, frequency, rate);

		// Create copy as arduinoFFT is an in-place implementation.
		double *data_r_cpy = (double *)malloc(data_size);
		memcpy(data_r_cpy, data_r, data_size);

		// Perform arduinoFFT
		auto ard_fft = arduinoFFT(data_r_cpy, data_i, n, rate);
		ard_fft.Compute(FFT_FORWARD);

		// Setup FFTW data
		double *data_out_r = (double *)malloc(data_size);
		double *data_out_i = (double *)malloc(data_size);

		// Perform FFTW
		test_fftw(data_r, data_out_r, data_out_i, n);

		for(size_t i = 0; i < n>>1; i++) {
			double_cast x;
			x.f = data_out_r[i];

			double_cast y;
			y.f = data_r_cpy[i];

			BOOST_CHECK(x.parts.exponent == y.parts.exponent);

			std::string x_str = std::to_string(x.parts.mantisa);
			std::string y_str = std::to_string(y.parts.mantisa);
			for(size_t m = 0; m < 3; m++) {
				BOOST_CHECK(x_str.c_str()[m] == y_str.c_str()[m]);
			}
		}

		#if GENERATE_OUTPUT == 1
			generate_output(data_out_r, data_out_i, data_r_cpy, data_i, n,
				OUTPUT_IMAGINARY);
		#endif

		free(data_r);
		free(data_i);
		free(data_r_cpy);
		free(data_out_r);
		free(data_out_i);
	}

	BOOST_AUTO_TEST_CASE(TestARDSeqSimple) {

		size_t n = 64;
		size_t data_size = sizeof(double) * n;
		size_t rate = 50000;
		double frequency = 1665.51254;

		// Generate real sine wave into data
		double *data_r = (double *)malloc(data_size);
		double *data_i = (double *)malloc(data_size);
		generate_sinewave(data_r, n, frequency, rate);

		// Create copy as arduinoFFT is an in-place implementation.
		double *data_r_cpy = (double *)malloc(data_size);
		memcpy(data_r_cpy, data_r, data_size);

		// Perform arduinoFFT
		auto ard_fft = arduinoFFT(data_r_cpy, data_i, n, rate);
		ard_fft.Windowing(FFT_WIN_TYP_NUTTALL, FFT_FORWARD);
		ard_fft.Compute(FFT_FORWARD);
		ard_fft.ComplexToMagnitude();

		// Setup FFTW data
		double *data_out_r = (double *)malloc(data_size);
		double *data_out_i = (double *)malloc(data_size);

		// Perform FFTW
		window_nuttall(data_r, n);
		test_fftw(data_r, data_out_r, data_out_i, n);
		complex_to_magnitude(data_out_r, data_out_i, n);

		for(size_t i = 0; i < n>>1; i++) {
			double_cast x;
			x.f = data_out_r[i];

			double_cast y;
			y.f = data_r_cpy[i];

			BOOST_CHECK(x.parts.exponent == y.parts.exponent);

			std::string x_str = std::to_string(x.parts.mantisa);
			std::string y_str = std::to_string(y.parts.mantisa);
			for(size_t m = 0; m < 3; m++) {
				BOOST_CHECK(x_str.c_str()[m] == y_str.c_str()[m]);
			}
		}

		#if GENERATE_OUTPUT == 2
			generate_output(data_out_r, data_out_i, data_r_cpy, data_i, n,
				OUTPUT_IMAGINARY);
		#endif

		free(data_r);
		free(data_i);
		free(data_r_cpy);
		free(data_out_r);
		free(data_out_i);
	}

	BOOST_AUTO_TEST_CASE(TestARDSeqCombine) {

		size_t n = 64;
		size_t data_size = sizeof(double) * n;
		size_t rate = 50000;
		double frequency = 1665.51254;

		// Generate real sine wave into data
		double *data_r = (double *)malloc(data_size);
		double *data_i = (double *)malloc(data_size);
		generate_sinewave(data_r, n, frequency, rate);

		double *data_r2 = (double *)malloc(data_size);
		double *data_r3 = (double *)malloc(data_size);

		generate_sinewave(data_r2, n, frequency * 4, rate);
		generate_sinewave(data_r3, n, frequency * 8, rate);

		for(size_t i = 0; i < n; i++) {
			data_r[i] += data_r2[i];
			data_r[i] += data_r3[i];
		}

		free(data_r2);
		free(data_r3);

		// Create copy as arduinoFFT is an in-place implementation.
		double *data_r_cpy = (double *)malloc(data_size);
		memcpy(data_r_cpy, data_r, data_size);

		// Perform arduinoFFT
		auto ard_fft = arduinoFFT(data_r_cpy, data_i, n, rate);
		ard_fft.Windowing(FFT_WIN_TYP_NUTTALL, FFT_FORWARD);
		ard_fft.Compute(FFT_FORWARD);
		ard_fft.ComplexToMagnitude();

		// Setup FFTW data
		double *data_out_r = (double *)malloc(data_size);
		double *data_out_i = (double *)malloc(data_size);

		// Perform FFTW
		window_nuttall(data_r, n);
		test_fftw(data_r, data_out_r, data_out_i, n);
		complex_to_magnitude(data_out_r, data_out_i, n);

		for(size_t i = 0; i < n>>1; i++) {
			double_cast x;
			x.f = data_out_r[i];

			double_cast y;
			y.f = data_r_cpy[i];

			BOOST_CHECK(x.parts.exponent == y.parts.exponent);

			std::string x_str = std::to_string(x.parts.mantisa);
			std::string y_str = std::to_string(y.parts.mantisa);
			for(size_t m = 0; m < 3; m++) {
				BOOST_CHECK(x_str.c_str()[m] == y_str.c_str()[m]);
			}
		}

		#if GENERATE_OUTPUT == 3
			generate_output(data_out_r, data_out_i, data_r_cpy, data_i, n,
				OUTPUT_IMAGINARY);
		#endif

		free(data_r);
		free(data_i);
		free(data_r_cpy);
		free(data_out_r);
		free(data_out_i);
	}

	BOOST_AUTO_TEST_CASE(TestARDSeqLimit) {

		size_t n = 64;
		size_t data_size = sizeof(double) * n;
		size_t rate = 50000;
		double frequency = 50000/2;

		// Generate real sine wave into data
		double *data_r = (double *)malloc(data_size);
		double *data_i = (double *)malloc(data_size);
		generate_sinewave(data_r, n, frequency, rate);

		// DC bias
		for(size_t i = 0; i < n; i++) {
			data_r[i] += 2.5;
		}

		// Create copy as arduinoFFT is an in-place implementation.
		double *data_r_cpy = (double *)malloc(data_size);
		memcpy(data_r_cpy, data_r, data_size);

		// Perform arduinoFFT
		auto ard_fft = arduinoFFT(data_r_cpy, data_i, n, rate);
		ard_fft.Windowing(FFT_WIN_TYP_NUTTALL, FFT_FORWARD);
		ard_fft.Compute(FFT_FORWARD);
		ard_fft.ComplexToMagnitude();

		// Setup FFTW data
		double *data_out_r = (double *)malloc(data_size);
		double *data_out_i = (double *)malloc(data_size);

		// Perform FFTW
		window_nuttall(data_r, n);
		test_fftw(data_r, data_out_r, data_out_i, n);
		complex_to_magnitude(data_out_r, data_out_i, n);

		for(size_t i = 0; i < n>>1; i++) {
			double_cast x;
			x.f = data_out_r[i];

			double_cast y;
			y.f = data_r_cpy[i];

			BOOST_CHECK(x.parts.exponent == y.parts.exponent);

			std::string x_str = std::to_string(x.parts.mantisa);
			std::string y_str = std::to_string(y.parts.mantisa);
			for(size_t m = 0; m < 3; m++) {
				BOOST_CHECK(x_str.c_str()[m] == y_str.c_str()[m]);
			}
		}

		#if GENERATE_OUTPUT == 4
			generate_output(data_out_r, data_out_i, data_r_cpy, data_i, n,
				OUTPUT_IMAGINARY);
		#endif

		free(data_r);
		free(data_i);
		free(data_r_cpy);
		free(data_out_r);
		free(data_out_i);
	}

	BOOST_AUTO_TEST_CASE(TestARDSeqDC) {

		size_t n = 64;
		size_t data_size = sizeof(double) * n;
		size_t rate = 50000;
		double frequency = 1665.51254;

		// Generate real sine wave into data
		double *data_r = (double *)malloc(data_size);
		double *data_i = (double *)malloc(data_size);
		generate_sinewave(data_r, n, frequency, rate);

		for(size_t i = 0; i < n; i++) {
			data_r[i] += 6.5;
		}

		// Create copy as arduinoFFT is an in-place implementation.
		double *data_r_cpy = (double *)malloc(data_size);
		memcpy(data_r_cpy, data_r, data_size);

		// Perform arduinoFFT
		auto ard_fft = arduinoFFT(data_r_cpy, data_i, n, rate);
		ard_fft.Windowing(FFT_WIN_TYP_NUTTALL, FFT_FORWARD);
		ard_fft.Compute(FFT_FORWARD);
		ard_fft.ComplexToMagnitude();

		// Setup FFTW data
		double *data_out_r = (double *)malloc(data_size);
		double *data_out_i = (double *)malloc(data_size);

		// Perform FFTW
		window_nuttall(data_r, n);
		test_fftw(data_r, data_out_r, data_out_i, n);
		complex_to_magnitude(data_out_r, data_out_i, n);

		for(size_t i = 0; i < n>>1; i++) {
			double_cast x;
			x.f = data_out_r[i];

			double_cast y;
			y.f = data_r_cpy[i];

			BOOST_CHECK(x.parts.exponent == y.parts.exponent);

			std::string x_str = std::to_string(x.parts.mantisa);
			std::string y_str = std::to_string(y.parts.mantisa);
			for(size_t m = 0; m < 3; m++) {
				BOOST_CHECK(x_str.c_str()[m] == y_str.c_str()[m]);
			}
		}

		#if GENERATE_OUTPUT == 5
			generate_output(data_out_r, data_out_i, data_r_cpy, data_i, n,
				OUTPUT_IMAGINARY);
		#endif

		free(data_r);
		free(data_i);
		free(data_r_cpy);
		free(data_out_r);
		free(data_out_i);
	}

BOOST_AUTO_TEST_SUITE_END()