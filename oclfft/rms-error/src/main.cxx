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

    for(size_t i = 0; i < 100; i++) {
        std::cout << "rng: " << rng.generate() << std::endl;
    }

//    std::cout << "pi         =    "<<    test            << std::endl;
//    std::cout << "pi*2       =    "<<    test*2          << std::endl;

//    // Compute all the vital characteristics of mpreal (in current precision)
//    // Analogous to lamch from LAPACK
//    const mpreal one         =    1.0;
//    const mpreal zero        =    0.0;
//    const mpreal eps         =    std::numeric_limits<mpreal>::epsilon();
//    const int    base        =    std::numeric_limits<mpreal>::radix;
//    const mpreal prec        =    eps * base;
//    const int bindigits      =    std::numeric_limits<mpreal>::digits(); // eqv. to mpfr::mpreal::get_default_prec();
//    const mpreal rnd         =    std::numeric_limits<mpreal>::round_error();
//    const mpreal maxval      =    std::numeric_limits<mpreal>::max();
//    const mpreal minval      =    std::numeric_limits<mpreal>::min();
//    const mpreal small       =    one / maxval;
//    const mpreal sfmin       =    (small > minval) ? small * (one + eps) : minval;
//    const mpreal round       =    std::numeric_limits<mpreal>::round_style();
//    const int    min_exp     =    std::numeric_limits<mpreal>::min_exponent;
//    const mpreal underflow   =    std::numeric_limits<mpreal>::min();
//    const int    max_exp     =    std::numeric_limits<mpreal>::max_exponent;
//    const mpreal overflow    =    std::numeric_limits<mpreal>::max();
//
//    // Additionally compute pi with required accuracy - just for fun :)
//    const mpreal pi          =    mpfr::const_pi();
//
//    cout.precision(digits);    // Show all the digits
//    cout << "pi         =    "<<    pi          << endl;
//    cout << "eps        =    "<<    eps         << endl;
//    cout << "base       =    "<<    base        << endl;
//    cout << "prec       =    "<<    prec        << endl;
//    cout << "b.digits   =    "<<    bindigits   << endl;
//    cout << "rnd        =    "<<    rnd         << endl;
//    cout << "maxval     =    "<<    maxval      << endl;
//    cout << "minval     =    "<<    minval      << endl;
//    cout << "small      =    "<<    small       << endl;
//    cout << "sfmin      =    "<<    sfmin       << endl;
//    cout << "1/sfmin    =    "<<    1 / sfmin   << endl;
//    cout << "round      =    "<<    round       << endl;
//    cout << "max_exp    =    "<<    max_exp     << endl;
//    cout << "min_exp    =    "<<    min_exp     << endl;
//    cout << "underflow  =    "<<    underflow   << endl;
//    cout << "overflow   =    "<<    overflow    << endl;

    return 0;
}
