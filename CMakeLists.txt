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

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(oCLFFT)

# 3.0 is minimum version as it is the first version to include FindOpenCL

set(PRJ_PRX ${CMAKE_PROJECT_NAME})
set(PRJ_LIB ${CMAKE_PROJECT_NAME}sup)

# Ensure the CMake target directory is not the same as the CMake root directory.
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR
    	"Please create a dedicated build directory. "
    	"(You may need remove the CmakeCache.txt and CMakeFiles)"
    )
ENDIF()

# Add custom cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(macros)

# Enable c++11 even when cmake < 3.1
use_cxx11()

# Enable pedantic errors and warnings for c++
use_cxx_warning_pedantic()

# Configurable options and defaults
OPTION(ENABLE_TESTS "Enable unit tests" ON)
OPTION(ENABLE_CODECOV "Measure code coverage" OFF)
OPTION(ENABLE_DOXYGEN "Generate documentation" ON)
OPTION(ENABLE_CI "Enable continuous integration" OFF)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)

# Required packages
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(OpenCL 2.0 REQUIRED)

# Include Commonly required headers
# include_directories(${ZeroMQ_INCLUDE_DIR})
# include_directories(${FairMQ_INCDIR}/fairmq) # INCDIR Triggered
# include_directories(${FairLogger_INCLUDE_DIR})
# include_directories(${ZOOKEEPER_INCLUDE_DIR})

# Set Commonly required libraries
set(${PRJ_PRX}_LIBRARIES
	Threads::Threads
	OpenCL::OpenCL
)

if(ENABLE_CODECOV)
	message(
		"${PRJ_PRX}: Running CodeCoverage, forcing ENABLE_TESTS & "
		"CMAKE_BUILD_TYPE=Debug"
	)
	include(CodeCoverage)
	setup_target_for_coverage(coverage testexample coverage)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
	set(ENABLE_TESTS ON) # tests need to be enabled for code coverage
	if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
		message(WARNING "Forcing build type to Debug to allow code coverage!")
		set(CMAKE_BUILD_TYPE Debug) # debug needed to disable compiler optimization
	endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(
		"${PRJ_PRX}: Disabling gcc optimization & adding extensive "
		"debug symbols"
	)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	message("${PRJ_PRX}: Enabling extensive optimizations")
	set(CMAKE_CXX_FLAGS 
		"${CMAKE_CXX_FLAGS} -Ofast -ffast-math -ftree-vectorize")
endif()

# Commonly shared library
add_subdirectory(lib)

# Collection of concrete implementations
add_subdirectory(oclfft)

# Toy experiments
add_subdirectory(playground)

# Unit tests to validate application
if(ENABLE_TESTS)
	message("${PRJ_PRX}: Enabling tests")
	ENABLE_TESTING()
	add_subdirectory(tests)
endif()

# Generate Doxygen Documentation
if(ENABLE_DOXYGEN)
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
		# set input and output files
		set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cnf)
		set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cnf)

		# request to configure the file
		configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
		message("${PRJ_PRX}: Doxygen build started")

		# note the option ALL which allows to build the docs together with the
		# application.
		if(NOT ENABLE_CI)
			add_custom_target(doxygen ALL
			COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating API documentation with Doxygen"
			VERBATIM )
		endif(NOT ENABLE_CI)
	else (DOXYGEN_FOUND)
		message(
			"${PRJ_PRX}: Doxygen needs to be installed to generate "
			"the documentation"
		)
	endif (DOXYGEN_FOUND)
endif()
