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

#ifndef APFFT_H
#define APFFT_H

#include <vector>
#include <mpreal.h>

typedef std::vector<mpfr::mpreal> apfft_data;

class APFFT {
public:
    APFFT(apfft_data *real, apfft_data *imag);
    void push();
    void synchronize();
    void window();
    void compute();
    void magnitude();
protected:
    const mpfr::mpreal TWO_PI = 2 * mpfr::const_pi(100);
    const mpfr::mpreal FOUR_PI = 4 * mpfr::const_pi(100);
    const mpfr::mpreal SIX_PI = 6 * mpfr::const_pi(100);

    template <class T> T sq(T x) {
        return x * x;
    }

    void Swap(mpfr::mpreal *x, mpfr::mpreal *y);

    size_t samples;
    size_t exponent;
    apfft_data *real;
    apfft_data *imag;
};

#endif // APFFT_H