# oCLFFT
# Copyright (C) 2025 Corne Lukken

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

project(hip-fft)

find_package(hipfft REQUIRED CONFIG PATHS)

set(${PRJ_PRX}_IMPL_LIBRARIES
    ${PRJ_LIB}
)

set(HIP_FFT_SOURCE
    src/main.cxx
    src/hip-fft.cxx
)

set(HIP_FFT_HEADERS
    include/hip-fft.hpp
)

message("${PRJ_PRX}: Hip FFT cxx flags:${CMAKE_CXX_FLAGS}")
add_executable(hip-fft ${HIP_FFT_SOURCE} ${HIP_FFT_HEADERS})

target_include_directories(hip-fft PRIVATE ${CMAKE_SOURCE_DIR}/lib/include)
target_include_directories(hip-fft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(${PRJ_PRX}_IMPL_LIBRARIES_PACK
    ${${PRJ_PRX}_LIBRARIES}
    ${${PRJ_PRX}_IMPL_LIBRARIES}
)

target_link_libraries(
    hip-fft
    PRIVATE hip::hipfft ${${PRJ_PRX}_IMPL_LIBRARIES_PACK}
)

add_library(hip-fft-shared SHARED ${HIP_FFT_SOURCE} ${HIP_FFT_HEADERS})
target_include_directories(hip-fft-shared PUBLIC ${CMAKE_SOURCE_DIR}/lib/include)
target_include_directories(hip-fft-shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(
    hip-fft-shared PUBLIC hip::hipfft ${${PRJ_PRX}_IMPL_LIBRARIES_PACK}
)