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

#include "results.hpp"

std::chrono::duration<double> Results::sum(size_t index) {
    using cdouble = std::chrono::duration<double, std::micro>;
    cdouble total {0.0};

    if(copy_host_to_device.size() > index)
        total += cdouble(copy_host_to_device[index]);
    if(window.size() > index)
        total += cdouble(window[index]);
    if(reverse.size() > index)
        total += cdouble(reverse[index]);
    if(fft.size() > index)
        total += cdouble(fft[index]);
    if(copy_device_to_host.size() > index)
        total += cdouble(copy_device_to_host[index]);

    return total;
}
