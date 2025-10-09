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

#include "rng.hpp"

MPRealRNG::MPRealRNG(float scale, size_t precision) {
    this->scale = scale;
    this->precision = precision;
    std::random_device rd;
    this->gen = std::mt19937(rd());

    // limit testing
//    mpfr::mpreal mp_scale = scale;
//    mpfr::mpreal mp_precision = precision;
    mpfr::mpreal mp_size_max = SIZE_MAX;
    this->digits = size_t(log10(mp_size_max));
//    std::cout << "Max digits per round: " << digits << std::endl;
//    if(scale * 2 * pow(10, precision) > mp_size_max) {
//        std::cerr << "[MPRealRNG] Scale factor: " << scale
//            << " to large for precision: " << precision << std::endl;
//        throw std::invalid_argument("Iek");
//    }
//    std::cout << "Scaling: " << scale * 2 * pow(10, precision) << std::endl;
    this->uniform = std::uniform_int_distribution<size_t>(1, SIZE_MAX);
}

mpfr::mpreal MPRealRNG::generate() {
    size_t rounds = precision / digits;
    size_t remainder = precision % digits;
    mpfr::mpreal result = 0;
    for(size_t i = 0; i < rounds; i++) {
        mpfr::mpreal digit = round();
        result += digit * pow(10, i*digits);
//        result = result << i*digits;
//        result += digit;
    }

    if(remainder) {
        auto remain = std::uniform_int_distribution<size_t>(1, pow(10, remainder));
        mpfr::mpreal digit = remain(gen);
        result += digit * pow(10, precision-remainder);
//        result = result << (precision-remainder);
//        result += digit;
    }

    return (result / pow(10, precision)) - 0.5;
}

mpfr::mpreal MPRealRNG::round() {
    return this->uniform(this->gen);
}

size_t MPRealRNG::digitsPerRound() {
    return this->digits;
}