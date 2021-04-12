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

#include "tests.hpp"

void generate_sinewave(
	double *data, size_t n, double frequency, size_t rate)
{
	double cycles = (((n-1) * frequency) / (double)rate);
	for (size_t i = 0; i < n; i++) {
		data[i] = (10 * (sin((i * (T_TWO_PI * cycles)) / n))) / 2.0;
	}
}

void complex_to_magnitude(double *data_r, double *data_i, size_t n) {
	for (size_t i = 0; i < n; i++) {
		data_r[i] = sqrt((data_r[i]*data_r[i]) + (data_i[i]*data_i[i]));
	}
}

void window_nuttall(double *data_r, size_t n) {
	const double samplesMinusOne = (double(n) - 1.0);

	for (size_t i = 0; i < (n >> 1); i++) {
		double ratio = ((double)i / samplesMinusOne);
		double weighingFactor = 0.355768 -
			(0.487396 * (cos(T_TWO_PI * ratio))) +
			(0.144232 * (cos(T_FOUR_PI * ratio))) -
			(0.012604 * (cos(T_SIX_PI * ratio)));
		data_r[i] *= weighingFactor;
		data_r[n - (i + 1)] *= weighingFactor;
	}
}

void bit_reversal(double *data_r, double *data_i, size_t n) {
	uint32_t j = 0;
	for (uint32_t i = 0; i < (n - 1); i++) {
		if (i < j) {
			double temp = data_r[i];
			data_r[i] = data_r[j];
			data_r[j] = temp;

			temp = data_i[i];
			data_i[i] = data_i[j];
			data_i[j] = temp;
		}

		// Reverse bits /
		uint32_t k = (n >> 1);
		while (k <= j) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}
}