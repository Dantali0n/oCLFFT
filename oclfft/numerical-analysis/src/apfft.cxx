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

#include "apfft.hpp"

APFFT::APFFT(apfft_data *real, apfft_data *imag) {
    this->real = real;
    this->imag = imag;
    this->samples = real->size();

    this->exponent = 0;
    while (((samples >> exponent) & 1) != 1) exponent++;
}

void APFFT::push() {

}

void APFFT::synchronize() {

}

void APFFT::window() {
    mpfr::mpreal samplesMinusOne = (mpfr::mpreal(samples) - 1.0);
    for (uint32_t i = 0; i < (samples >> 1); i++) {
        mpfr::mpreal indexMinusOne = mpfr::mpreal(i);
        mpfr::mpreal ratio = (indexMinusOne / samplesMinusOne);
        mpfr::mpreal weighingFactor = 0.355768 - (0.487396 * (cos(TWO_PI * ratio))) + (0.144232 * (cos(FOUR_PI * ratio))) - (0.012604 * (cos(SIX_PI * ratio)));
        (*real)[i] *= weighingFactor;
        (*real)[samples - (i + 1)] *= weighingFactor;
    }
}

void APFFT::compute() {
    // reorder //
    uint32_t j = 0;
    for (uint32_t i = 0; i < (samples - 1); i++) {
        if (i < j) {
            Swap(&(*real)[i], &(*real)[j]);
            Swap(&(*imag)[i], &(*imag)[j]);
        }

        // Reverse bits /
        uint32_t k = (samples >> 1);
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }
    // Compute the FFT  /
    mpfr::mpreal c1 = -1.0;
    mpfr::mpreal c2 = 0.0;
    uint32_t l2 = 1;
    for (uint8_t l = 0; (l < exponent); l++) {
        uint32_t l1 = l2;
        l2 <<= 1;
        //printf("l2: %u\n", l2);
        mpfr::mpreal u1 = 1.0;
        mpfr::mpreal u2 = 0.0;
        for (j = 0; j < l1; j++) {
            for (uint32_t i = j; i < samples; i += l2) {
                uint32_t i1 = i + l1;
                mpfr::mpreal t1 = u1 * (*real)[i1] - u2 * (*imag)[i1];
                mpfr::mpreal t2 = u1 * (*imag)[i1] + u2 * (*real)[i1];
                (*real)[i1] = (*real)[i] - t1;
                (*imag)[i1] = (*imag)[i] - t2;
                (*real)[i] += t1;
                (*imag)[i] += t2;
            }
            mpfr::mpreal z = ((u1 * c1) - (u2 * c2));
            u2 = ((u1 * c2) + (u2 * c1));
            u1 = z;
        }
        c2 = sqrt((1.0 - c1) / 2.0);
        c1 = sqrt((1.0 + c1) / 2.0);
        c2 = -c2;
    }
}

void APFFT::magnitude() {
    for (uint32_t i = 0; i < samples; i++) {
        (*real)[i] = sqrt(((*real)[i] * (*real)[i]) + ((*imag)[i] * (*imag)[i]));
    }
}

void APFFT::Swap(mpfr::mpreal *x, mpfr::mpreal *y) {
    std::complex<mpfr::mpreal> temp = *x;
    *x = *y;
    *y = temp;
}