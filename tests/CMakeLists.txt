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

project(tests)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${FFTW_INCLUDES})

set(CMAKE_OG_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_OG_LINKER_FLAGS "${CMAKE_LINKER_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")

find_package(
    Boost 1.53 COMPONENTS
    timer
    unit_test_framework
    REQUIRED
)

find_package(FFTW REQUIRED)

set(${PRJ_PRX}_TEST_LIBRARIES
	${PRJ_LIB}
	Boost::timer
	Boost::unit_test_framework
	${FFTW_LIBRARIES}
)

set(${PRJ_PRX}_LIBRARIES_PACK
	${${PRJ_PRX}_LIBRARIES}
	${${PRJ_PRX}_TEST_LIBRARIES}
)



set(LIB_DFT_SRC
	${CMAKE_SOURCE_DIR}/oclfft/dft-seq/src/dft-seq.cxx
)

set(LIB_DFT_HEADERS
	${CMAKE_SOURCE_DIR}/oclfft/dft-seq/include/dft-seq.hpp
)

add_library(testdft SHARED ${LIB_DFT_SRC} ${LIB_DFT_HEADERS})
set_target_properties(testdft PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${CMAKE_SOURCE_DIR}/oclfft/dft-seq/include)
set(TEST_DFTSEQ_SOURCE
	src/test-dft-seq.cxx
	src/fftw.cxx
	src/tests.cxx
)

set(TEST_DFTSEQ_HEADERS
	include/tests.hpp
)

message("${PRJ_PRX}: test dft-seq cxx flags:${CMAKE_CXX_FLAGS}")
add_executable(testdftseq ${TEST_DFTSEQ_SOURCE} ${TEST_DFTSEQ_HEADERS})

target_link_libraries(
	testdftseq
	testdft
	${${PRJ_PRX}_LIBRARIES_PACK}
)



set(LIB_ARD_SRC
	${CMAKE_SOURCE_DIR}/oclfft/ard-seq/src/ard-seq.cxx
)

set(LIB_ARD_HEADERS
	${CMAKE_SOURCE_DIR}/oclfft/ard-seq/include/ard-seq.hpp
)

add_library(testard SHARED ${LIB_ARD_SRC} ${LIB_ARD_HEADERS})
set_target_properties(testard PROPERTIES LINKER_LANGUAGE CXX)

include_directories(${CMAKE_SOURCE_DIR}/oclfft/ard-seq/include)
set(TEST_ARDSEQ_SOURCE
	src/test-ard-seq.cxx
	src/fftw.cxx
	src/tests.cxx
)

set(TEST_ARDSEQ_HEADERS
	include/tests.hpp
)

message("${PRJ_PRX}: test ard-seq cxx flags:${CMAKE_CXX_FLAGS}")
add_executable(testardseq ${TEST_ARDSEQ_SOURCE} ${TEST_ARDSEQ_HEADERS})

target_link_libraries(
	testardseq
	testard
	${${PRJ_PRX}_LIBRARIES_PACK}
)



# restore flags as OpenCL is incompatible with address sanitzer
set(CMAKE_CXX_FLAGS "${CMAKE_OG_CXX_FLAGS}")
set(CMAKE_LINKER_FLAGS "${CMAKE_OG_LINKER_FLAGS}")

# Run kernel.sh to let objcopy turn opencl into data segments
set(kernel-data "${CMAKE_SOURCE_DIR}/oclfft/ard-ocl/src/kernel.o")
add_custom_command(OUTPUT ${kernel-data}
	COMMAND ${CMAKE_SOURCE_DIR}/oclfft/ard-ocl/kernel.sh
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/oclfft/ard-ocl"
)

set(lookup-data "${CMAKE_SOURCE_DIR}/oclfft/ard-ocl/src/lookup.o")
add_custom_command(OUTPUT ${lookup-data}
	COMMAND ${CMAKE_SOURCE_DIR}/oclfft/ard-ocl/lookup.sh
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/oclfft/ard-ocl"
)

set(LIB_ARD_OCL_SRC
	${CMAKE_SOURCE_DIR}/oclfft/ard-ocl/src/ard-ocl.cxx
	${kernel-data}
	${lookup-data}
)

set(LIB_ARD_OCL_HEADERS
	${CMAKE_SOURCE_DIR}/oclfft/ard-ocl/include/ard-ocl.hpp
)

add_library(testaocl SHARED ${LIB_ARD_OCL_SRC} ${LIB_ARD_OCL_HEADERS})
set_target_properties(testaocl PROPERTIES LINKER_LANGUAGE CXX)

# copy the compiled kernel to the test directory
# remove the objcopy source file after compilation so it always gets recompiled
add_custom_command(TARGET testaocl
	POST_BUILD
	COMMAND rm ${CMAKE_SOURCE_DIR}/oclfft/ard-ocl/src/kernel.o
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

include_directories(${CMAKE_SOURCE_DIR}/oclfft/ard-ocl/include)
set(TEST_ARD_OCL_SOURCE
	src/test-ard-ocl.cxx
	src/fftw.cxx
	src/tests.cxx
)

set(TEST_ARD_OCL_HEADERS
	include/tests.hpp
)

message("${PRJ_PRX}: test ard-ocl cxx flags:${CMAKE_CXX_FLAGS}")
add_executable(testardocl ${TEST_ARD_OCL_SOURCE} ${TEST_ARD_OCL_HEADERS})

target_link_libraries(
	testardocl
	testaocl
	${${PRJ_PRX}_LIBRARIES_PACK}
)



add_test(TestExample testexample)
add_test(TestDFTSeq testdftseq)
add_test(TestARDSeq testardseq)
add_test(TestARDocl testardocl)