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
__constant float PI = M_PI;
__constant float TWO_PI = 2*M_PI;
__constant float FOUR_PI = 4*M_PI;
__constant float SIX_PI = 6*M_PI;

__constant float *U1[] = {
    u10, u11, u12, u13, u14, u15, u16, u17, u18, u19, u110, u111, u112,
    u113, u114, u115, u116, u117, u118, u119, u120, u121
};

__constant float *U2[] = {
    u20, u21, u22, u23, u24, u25, u26, u27, u28, u29, u210, u211, u212,
    u213, u214, u215, u216, u217, u218, u219, u220, u221
};

void kernel print_layout() {
    printf("[0]G[%lu]L[%lu] - [1]G[%lu]L[%lu]\n", get_group_id(0), get_local_id(0), get_group_id(1), get_local_id(1));
//    printf("[1]G[%lu]L[%lu]\n", get_group_id(1), get_local_id(1));
//    printf("[2]G[%lu]L[%lu]\n", get_group_id(2), get_local_id(2));
}

/**
 * First executed kernel has drastically lower overall performance.
 * Submit this kernel as no-op to prevent impact
 */
void kernel dummy_operation() {
    const size_t i = get_group_id(0)  + get_local_id(0);
    float temp = cos(TWO_PI * i);
}

void kernel window(global float *real, global float *imag, ulong size, const ulong wavefront) {
    const size_t n = size;
    const size_t i = get_group_id(0) * wavefront + get_local_id(0);
    float samplesMinusOne = (convert_float(n) - 1.0);

    float ratio = ((float)i / samplesMinusOne);
    float weighingFactor = 0.355768 - (0.487396 * (cos(TWO_PI * ratio))) + (0.144232 * (cos(FOUR_PI * ratio))) - (0.012604 * (cos(SIX_PI * ratio)));
    real[i] = real[i] * weighingFactor;
    real[n - (i + 1)] = real[n - (i + 1)] * weighingFactor;
}

void kernel bit_column(global float *real, global float *imag, const global uint *lookup, const ulong height, const ulong wavefront) {
    const size_t x = get_group_id(0) * 2 + get_local_id(0) + 1;
    const size_t y = get_group_id(1) * wavefront + get_local_id(1);

//    printf("[%lu][%lu]\n", x, y);

    // skip entire columns per x
    const size_t xh = x*height;
    // combined offset for block
    const size_t xyh = xh+y;

    // Find swap and only proceed if lookup result is different from self
    const size_t lk = lookup[x];
    if(lk != x && x < lk) {

        // swap destination column
        const size_t swap = lookup[x]*height+y;

        float temp = real[xyh];
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
    const ulong P, const ulong Q, const global float *input,
    global float *output
) {

    // Thread identifiers
    const size_t tx = get_local_id(0);
    const size_t ty = get_local_id(1);
    const size_t ID0 = get_group_id(0)*TRANSPOSEX + tx; // 0..P
    const size_t ID1 = get_group_id(1)*TRANSPOSEY + ty; // 0..Q
    //printf("tx %lu, ty %lu, id0 %lu, id1 %lu\n", tx, ty, ID0, ID1);

    // Set-up the local memory for shuffling
    local float buffer[TRANSPOSEX][TRANSPOSEY];

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

void kernel fft_pow(global float *real, global float *imag, ulong power, ulong l1, ulong l2, float c1, float c2, const ulong x_dim, const ulong y_dim) {

    ulong j = get_group_id(0) * x_dim + get_local_id(0);

    float u1 = (U1[power])[j];

    ulong i = ((get_group_id(1) * y_dim + get_local_id(1)) * l2) + j;

    float u2 = (U2[power])[j];

    ulong i1 = i + l1;
    float t1 = u1 * real[i1] - u2 * imag[i1];
    float t2 = u1 * imag[i1] + u2 * real[i1];

    real[i1] = real[i] - t1;
    imag[i1] = imag[i] - t2;
    real[i] += t1;
    imag[i] += t2;
}

void kernel magnitude(global float *real, global float *imag, const ulong wavefront) {
    ulong i = get_group_id(0) * wavefront + get_local_id(0);
    real[i] = sqrt((real[i] * real[i]) + (imag[i] * imag[i]));
}