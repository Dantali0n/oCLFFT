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

// Use local memory as it fast enough to not limit performance
__constant double PI = M_PI;
__constant double TWO_PI = 2*M_PI;
__constant double FOUR_PI = 4*M_PI;
__constant double SIX_PI = 6*M_PI;

__constant double *U1[] = {
    u10, u11, u12, u13, u14, u15, u16, u17, u18, u19, u110, u111, u112,
    u113, u114, u115, u116, u117, u118, u119, u120, u121
};

__constant double *U2[] = {
    u20, u21, u22, u23, u24, u25, u26, u27, u28, u29, u210, u211, u212,
    u213, u214, u215, u216, u217, u218, u219, u220, u221
};

void kernel print_layout() {
    printf("G[%lu]L[%lu]\n", get_global_id(0), get_local_id(0));
}

void kernel window(global double *real, global double *imag, ulong size) {
    const size_t n = size;
    const size_t i = get_global_id(0);
    //printf("id: %u\n", i);
    const size_t n_half = n >> 1;
    double samplesMinusOne = (convert_double(n) - 1.0);

    double ratio = ((double)get_global_id(0) / samplesMinusOne);
    double weighingFactor = 0.355768 - (0.487396 * (cos(TWO_PI * ratio))) + (0.144232 * (cos(FOUR_PI * ratio))) - (0.012604 * (cos(SIX_PI * ratio)));
    real[i] = real[i] * weighingFactor;
    real[n - (i + 1)] = real[n - (i + 1)] * weighingFactor;
}

void kernel bit_reverse(global double *real, global double *imag, ulong size) {
    const size_t n = size;
    const size_t nminone = size-1;
    const size_t n_half = n >> 1;

    size_t j = 0;

    for (size_t i =  get_global_id(0); i < nminone; i++) {
        //printf("i: %lu", i);
        //printf("j: %lu", j);
        if (i < j) {
            double temp = real[i];
            real[i] = real[j];
            real[j] = temp;

            temp = imag[i];
            imag[i] = imag[j];
            imag[j] = temp;
        }

        size_t k = n_half;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }
}

void kernel bit_column(global double *real, global double *imag, const global uint *lookup, const ulong height) {
    const size_t x = get_global_id(0)+1;
    const size_t y = get_global_id(1);

    // skip entire columns per x
    const size_t xh = x*height;
    // combined offset for block
    const size_t xyh = xh+y;

    // Find swap and only proceed if lookup result is different from self
    const size_t lk = lookup[x];
    if(lk != x && x < lk) {

        // swap destination column
        const size_t swap = lookup[x]*height+y;

        double temp = real[xyh];
        real[xyh] = real[swap];
        real[swap] = temp;

        temp = imag[xyh];
        imag[xyh] = imag[swap];
        imag[swap] = temp;
    }
}

#define TRANSPOSEX 16
#define TRANSPOSEY 16

/**
 * Original author: Cedric Nugteren, SURFsara
 * https://github.com/CNugteren/myGEMM/blob/master/src/kernels.cl#L1247
 */
kernel void transpose(
    const ulong P, const ulong Q, const global double *input,
    global double *output
) {

    // Thread identifiers
    const size_t tx = get_local_id(0);
    const size_t ty = get_local_id(1);
    const size_t ID0 = get_group_id(0)*TRANSPOSEX + tx; // 0..P
    const size_t ID1 = get_group_id(1)*TRANSPOSEY + ty; // 0..Q
    //printf("tx %lu, ty %lu, id0 %lu, id1 %lu\n", tx, ty, ID0, ID1);

    // Set-up the local memory for shuffling
    local double buffer[TRANSPOSEX][TRANSPOSEY];

    // Swap the x and y coordinates to perform the rotation (coalesced)
    if (ID0 < P && ID1 < Q) {
        buffer[ty][tx] = input[ID1*P + ID0];
    }

    // Synchronise all threads
    barrier(CLK_LOCAL_MEM_FENCE);

    // We don't have to swap the x and y thread indices here,
    // because that's already done in the local memory
    const size_t newID0 = get_group_id(1)*TRANSPOSEX + tx;
    const size_t newID1 = get_group_id(0)*TRANSPOSEY + ty;

    // Store the transposed result (coalesced)
    if (newID0 < Q && newID1 < P) {
        output[newID1*Q + newID0] = buffer[tx][ty];
    }
}

void kernel fft_pow(global double *real, global double *imag, ulong power, ulong l1, ulong l2, double c1, double c2) {
    ulong j = get_global_id(1);

    double u1 = (U1[power])[j];

    ulong i = (get_global_id(0) * l2) + j;

    double u2 = (U2[power])[j];

    ulong i1 = i + l1;
    double t1 = u1 * real[i1] - u2 * imag[i1];
    double t2 = u1 * imag[i1] + u2 * real[i1];

    real[i1] = real[i] - t1;
    imag[i1] = imag[i] - t2;
    real[i] += t1;
    imag[i] += t2;
}

void kernel magnitude(global double *real, global double *imag) {
    ulong i = get_global_id(0);
    real[i] = sqrt((real[i] * real[i]) + (imag[i] * imag[i]));
}