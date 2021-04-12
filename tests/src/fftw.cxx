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

#include <fftw3.h>

void test_fftw(double *in, double *out_r, double *out_i, size_t n) {
	fftw_complex *fw_in, *fw_out;
	fftw_plan p;

	fw_in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * n);
	fw_out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * n);
	p = fftw_plan_dft_1d(n, fw_in, fw_out, FFTW_FORWARD, FFTW_ESTIMATE);

	for(size_t i = 0; i < n; i++) {
		fw_in[i][0] = in[i];
		fw_in[i][1] = 0.0;
	}

	fftw_execute(p);

	for(size_t i = 0; i < n>>1; i++) {
		out_r[i] = fw_out[i][0];
		out_i[i] = fw_out[i][1];
	}

	fftw_destroy_plan(p);
	fftw_free(fw_in);
	fftw_free(fw_out);
}