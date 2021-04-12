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

project(dft-seq)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -ffast-math -ftree-vectorize")

set(${PRJ_PRX}_IMPL_LIBRARIES
    ${PRJ_LIB}
)

set(DFT_SEQ_SOURCE
    src/dft-seq.cxx
    src/main.cxx
)

set(DFT_SEQ_HEADERS
    include/dft-seq.hpp
)

message("${PRJ_PRX}: DFT sequential cxx flags:${CMAKE_CXX_FLAGS}")
add_executable(dft-seq ${DFT_SEQ_SOURCE} ${DFT_SEQ_HEADERS})


set(${PRJ_PRX}_IMPL_LIBRARIES_PACK
    ${${PRJ_PRX}_LIBRARIES}
    ${${PRJ_PRX}_IMPL_LIBRARIES}
)

target_link_libraries(
    dft-seq
    ${${PRJ_PRX}_IMPL_LIBRARIES_PACK}
)