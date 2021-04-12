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

project(${PRJ_LIB})

find_package(
    Boost 1.32 COMPONENTS
    program_options
    REQUIRED
)

find_package(FFTW REQUIRED)

set(${PRJ_LIB}_LIBRARIES
    Boost::program_options
    ${FFTW_LIBRARIES}
)

set(LIB_${PRJ_LIB}_SRC
    src/oclfft.cxx
    src/parsing.cxx
)

set(LIB_${PRJ_LIB}_HEADERS.
    include/oclfft.hpp
    include/parsing.hpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(
    ${PRJ_LIB} SHARED
    ${LIB_${PRJ_LIB}_SRC}
    ${LIB_${PRJ_LIB}_HEADERS}
)
set_target_properties(${PRJ_LIB} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(
    ${PRJ_LIB}
    ${${PRJ_LIB}_LIBRARIES}
)
