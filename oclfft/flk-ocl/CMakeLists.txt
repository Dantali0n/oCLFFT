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

project(flk-ocl)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(${PRJ_PRX}_IMPL_LIBRARIES
    ${PRJ_LIB}
)

# Run data.sh to let objcopy turn opencl into data segments
set(kernel-data "${CMAKE_CURRENT_SOURCE_DIR}/src/kernel.o")
add_custom_command(OUTPUT ${kernel-data}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/kernel.sh
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(lookup-data "${CMAKE_CURRENT_SOURCE_DIR}/src/lookup.o")
add_custom_command(OUTPUT ${lookup-data}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lookup.sh
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(FLK_OCL_SOURCE
    src/main.cxx
    src/flk-ocl.cxx
    ${kernel-data}
    ${lookup-data}
)

set(FLK_OCL_HEADERS
    include/flk-ocl.hpp
)

message("${PRJ_PRX}: Fast-Lookup OpenCL cxx flags:${CMAKE_CXX_FLAGS}")
add_executable(flk-ocl ${FLK_OCL_SOURCE} ${FLK_OCL_HEADERS})

set(${PRJ_PRX}_IMPL_LIBRARIES_PACK
    ${${PRJ_PRX}_LIBRARIES}
    ${${PRJ_PRX}_IMPL_LIBRARIES}
)

target_link_libraries(
    flk-ocl
    ${${PRJ_PRX}_IMPL_LIBRARIES_PACK}
)

# remove the objcopy source file after compilation so it always gets recompiled
add_custom_command(TARGET flk-ocl
    POST_BUILD
    COMMAND rm ${CMAKE_CURRENT_SOURCE_DIR}/src/kernel.o
    COMMAND rm ${CMAKE_CURRENT_SOURCE_DIR}/src/lookup.o
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
