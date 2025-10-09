# oCLFFT
# Copyright (C) 2024 Corne Lukken

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

project(numerical-analysis)

find_package(MPFR REQUIRED)

find_package(
    Boost 1.32 CONFIG COMPONENTS
    program_options
    REQUIRED
)

find_package(FFTW REQUIRED)

set(${PRJ_PRX}_IMPL_LIBRARIES
    Boost::program_options
    ${MPFR_LIBRARIES}
    ${FFTW_LIBRARIES}
)

set(NUMERICAL_ANALYSIS_SOURCE
    src/apfft.cxx
    src/main.cxx
    src/parsing.cxx
    src/rng.cxx
)

set(NUMERICAL_ANALYSIS_HEADERS
    include/apfft.hpp
    include/defaults.hpp
    include/main.hpp
    include/options.hpp
    include/parsing.hpp
)

message("${PRJ_PRX}: RMS error cxx flags:${CMAKE_CXX_FLAGS}")
add_executable(numerical-analysis ${NUMERICAL_ANALYSIS_SOURCE} ${NUMERICAL_ANALYSIS_HEADERS})

target_include_directories(numerical-analysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(
    numerical-analysis
    PUBLIC flk-ocl-fp32-shared # hip-fft-shared
    PRIVATE ${${PRJ_PRX}_IMPL_LIBRARIES}
)
