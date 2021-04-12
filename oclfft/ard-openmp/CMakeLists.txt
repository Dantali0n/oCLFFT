# oCLFFT
# Copyright (C) 2021 Corne Lukken

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

CMAKE_MINIMUM_REQUIRED(VERSION 3.9)
project(ard-omp)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(OpenMP REQUIRED)

set(${PRJ_PRX}_IMPL_LIBRARIES
    OpenMP::OpenMP_CXX
    ${PRJ_LIB}
)

set(ARD_OMP_SOURCE
    src/main.cxx
    src/ard-omp.cxx
)

set(ARD_OMP_HEADERS
    include/ard-omp.hpp
)

message("${PRJ_PRX}: DFT openmp cxx flags:${CMAKE_CXX_FLAGS}")
add_executable(ard-omp ${ARD_OMP_SOURCE} ${ARD_OMP_HEADERS})


set(${PRJ_PRX}_IMPL_LIBRARIES_PACK
    ${${PRJ_PRX}_LIBRARIES}
    ${${PRJ_PRX}_IMPL_LIBRARIES}
)

target_link_libraries(
    ard-omp
    ${${PRJ_PRX}_IMPL_LIBRARIES_PACK}
)