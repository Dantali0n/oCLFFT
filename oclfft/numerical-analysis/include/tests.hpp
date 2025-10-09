/*
oCLFFT
    Copyright (C) 2025 Corne Lukken

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
#include "options.hpp"

/**
 * Generate test data
 * @param opts Selected options including the test and number of samples
 * @param real Pointer to populate with real values
 * @param imag Pointer to populate with imaginary values
 */
void populate_test_data(options opts, apfft_data *real, apfft_data *imag);