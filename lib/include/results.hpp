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

#ifndef OCLFFT_RESULTS_HPP
#define OCLFFT_RESULTS_HPP

#include <chrono>
#include <ratio>
#include <cstdint>
#include <vector>

struct Results {
    std::vector<int64_t> copy_host_to_device = std::vector<int64_t>();
    std::vector<int64_t> window = std::vector<int64_t>();
    std::vector<int64_t> reverse = std::vector<int64_t>();
    std::vector<int64_t> fft = std::vector<int64_t>();
    std::vector<int64_t> magnitude = std::vector<int64_t>();
    std::vector<int64_t> copy_device_to_host = std::vector<int64_t>();

    std::chrono::duration<double> sum(size_t index);
};

#endif // OCLFFT_RESULTS_HPP