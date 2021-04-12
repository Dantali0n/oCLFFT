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

#include "dft-seq.hpp"

void dftseq::synchronize() {

}

void dftseq::window() {
	const size_t n = this->data->size();
	const size_t n_half = n >> 1;
	double samplesMinusOne = (double(n) - 1.0);

	for (size_t i = 0; i < n_half; i++) {
		double ratio = ((double)i / samplesMinusOne);
		double weighingFactor = 0.355768 - (0.487396 * (cos(TWO_PI * ratio))) + (0.144232 * (cos(FOUR_PI * ratio))) - (0.012604 * (cos(SIX_PI * ratio)));
		(*this->data)[i].real((*this->data)[i].real() * weighingFactor);
		(*this->data)[n - (i + 1)].real((*this->data)[n - (i + 1)].real() * weighingFactor);
	}
}

void dftseq::compute() {
	size_t n = this->data->size();
	double two_pi_n = TWO_PI / n;

	auto copy = std::vector<std::complex<double>>();

	for(size_t k = 0; k < n; k++) { // n^1
		auto result = std::complex<double>(0, 0); // n^2
		copy.emplace_back();
		for(size_t j = 0; j < n; j++) {
			size_t kj = k * j;
			result.real(
				(*this->data)[j].real() *
				cos(two_pi_n * kj) +
				(*this->data)[j].imag() *
				sin(two_pi_n * kj)
			);

			result.imag(
				(*this->data)[j].real() *
				-sin(two_pi_n * kj) +
				(*this->data)[j].imag() *
				cos(two_pi_n * kj)
			);
			copy[k] += result;
		}
	}

	for(size_t i = 0; i < n; i++) { // n^3
		(*this->data)[i] = copy[i];
	}
}

void dftseq::magnitude() {
	for (auto &value : *this->data) {
		value.real(sqrt(sq(value.real()) + sq(value.imag())));
	}
}