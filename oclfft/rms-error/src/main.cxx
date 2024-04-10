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

using mpfr::mpreal;

int main(int argc, char* argv[]) {
    apfft_data data;
    struct options opts{};
    parse_args(argc, argv, &opts);

    // Setup default precision for all subsequent computations
    // MPFR accepts precision in bits - so we do the conversion
    mpreal::set_default_prec(mpfr::digits2bits(opts.precision));
    std::cout.precision(opts.precision);

    MPRealRNG rng(opts.scale, opts.precision);


    for(size_t i = 0; i < opts.samples; i++) {

    }

//    APFFT fft();

    std::cout << "\t oCLFFT relative RMS error calculator:" << std::endl;
    std::cout << "  Samples: " << opts.samples << std::endl;
    std::cout << "Precision: " << opts.precision << std::endl;
    std::cout << "    Scale: " << opts.scale << std::endl;

    for(size_t i = 0; i < 100000; i++) {
        std::cout << "" << rng.generate() << std::endl;
    }

    return 0;
}
