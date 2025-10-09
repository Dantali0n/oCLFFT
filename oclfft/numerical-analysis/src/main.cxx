/*
	oCLFFT
	Copyright (C) 2024 Corne Lukken

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

#include <iostream>
#include <mpreal.h>

#include "parsing.hpp"
#include "options.hpp"
#include "apfft.hpp"
#include "rng.hpp"

#include "oclfft.hpp"
// #include "hip-fft.hpp"
// #include "flk-ocl.hpp"
#include "flk-ocl32.hpp"
#include "results.hpp"

using mpfr::mpreal;

int main(int argc, char* argv[]) {
    apfft_data real, imag, original_real, original_imag;
    struct options opts{};
    parse_args(argc, argv, &opts);

	std::cout << "\t oCLFFT numerical analysis:" << std::endl;
	std::cout << "  Samples: " << opts.samples << std::endl;
	std::cout << "Precision: " << opts.precision << std::endl;
	std::cout << "    Scale: " << opts.scale << std::endl;

    // Setup default precision for all subsequent computations
    // MPFR accepts precision in bits - so we do the conversion
    mpreal::set_default_prec(mpfr::digits2bits(opts.precision));
	// mpreal::set_default_prec(100);
    std::cout.precision(opts.precision);

	// TODO: Support multiple test data patterns
	// TODO: Move to tests
    MPRealRNG rng(opts.scale, opts.precision);
    for(size_t i = 0; i < opts.samples; i++) {
		original_real.push_back(rng.generate());
		original_imag.push_back(rng.generate());
    }

	// Copy the data for APFFT
	real = original_real;
	imag = original_imag;

    APFFT fft(&real, &imag);

	fft.compute();

	auto data = std::vector<std::complex<double>>();
	for(size_t i = 0; i < opts.samples; i++) {
		data.push_back(std::complex<double>(
			static_cast<double>(original_real[i]), static_cast<double>(original_imag[i]))
		);
	}
	auto results = Results();
	oclfft::options ocl_opts{};
	// // Compatability preferred over speed here
	ocl_opts.wavefront = 8;
	ocl_opts.iterations = 1;
	ocl_opts.samples = opts.samples;
	auto focl = FlkOCL32(&data, ocl_opts, &results);
	// auto focl = HipFFT(&data, ocl_opts, &results);
	// // auto focl = HipFFT(&data, ocl_opts, &results);
	focl.push();
	focl.reverse();
	focl.compute();
	focl.synchronize();
	// fftw_compute_fp32(&data);

	/**
	 * relative = (a - b) / b
	 * norm2 = np.sqrt(np.sum(np.abs(x)**2))             # ℓ₂‑norm
	 * rms   = np.sqrt(np.mean(np.abs(x)**2))		     # RMS
	 *
	 */
	mpfr::mpreal n1 = 0.0f;
	mpfr::mpreal n2 = 0.0f;
	mpfr::mpreal ninf = 0.0f;
	mpfr::mpreal l1 = 0.0f;
	mpfr::mpreal l2 = 0.0f;
	mpfr::mpreal linf = 0.0f;
	mpfr::mpreal ntemp = 0.0f;
	mpfr::mpreal ltemp = 0.0f;
	// Ln = ||x||n = (sum |xi|^n)^1/n
	for (size_t i = 0; i < opts.samples; i++){
		ltemp =
			mpfr::pow(data[i].real() - real[i], 2) +
			mpfr::pow(data[i].imag() - imag[i], 2);
		ntemp = mpfr::pow(real[i], 2) + mpfr::pow(imag[i], 2);
		if (ntemp > ninf) ninf = ntemp;
		if (ltemp > linf) linf = ltemp;
		n1 += mpfr::sqrt(ntemp);
		n2 += ntemp;
		l1 += mpfr::sqrt(ltemp);
		l2 += ltemp;
	}
	// Compare(a, b) = ||a – b||n / ||b||n
	std::cout << "L1: " << l1 / n1 << std::endl;
	std::cout << "L2: " << mpfr::sqrt(l2 / n2) << std::endl;
	std::cout << "Linf: " << mpfr::sqrt(linf / ninf) << std::endl;
	std::cout << "RMS: " << mpfr::sqrt(l2 / real.size()) << std::endl;

    return 0;
}
