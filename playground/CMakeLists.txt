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

project(playground)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)

set(${PRJ_PRX}_PLAYGROUND_LIBRARIES
    ${PRJ_LIB}
)

set(${PRJ_PRX}_PLAY_LIBRARIES_PACK
    ${${PRJ_PRX}_LIBRARIES}
    ${${PRJ_PRX}_PLAYGROUND_LIBRARIES}
)

set(PLAYGROUND_EXAMPLE_SOURCE
    example/playground-example.cxx
)

message("${PRJ_PRX}: playground example cxx flags:${CMAKE_CXX_FLAGS}")
add_executable(play-example ${PLAYGROUND_EXAMPLE_SOURCE})

target_link_libraries(
    play-example
    ${${PRJ_PRX}_PLAY_LIBRARIES_PACK}
)

# Run data.sh to let objcopy turn opencl into data segments
set(objcopy-data "${CMAKE_CURRENT_SOURCE_DIR}/objcopy/data.o")
add_custom_command(OUTPUT ${objcopy-data}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/objcopy/data.sh
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(PLAYGROUND_OBJCOPY_SOURCE
    objcopy/objcopy-data.cxx
    ${objcopy-data}
)

message("${PRJ_PRX}: playground objcopy cxx flags:${CMAKE_CXX_FLAGS}")
add_executable(play-objcopy ${PLAYGROUND_OBJCOPY_SOURCE})

target_link_libraries(
    play-objcopy
    ${${PRJ_PRX}_PLAY_LIBRARIES_PACK}
)

# remove the objcopy source file after compilation so it always gets recompiled
add_custom_command(TARGET play-objcopy
    POST_BUILD
    COMMAND rm ${CMAKE_CURRENT_SOURCE_DIR}/objcopy/data.o
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(PLAYGROUND_LOOKUP_SOURCE
    clookup/clookup.cxx
)

message("${PRJ_PRX}: playground lookup cxx flags:${CMAKE_CXX_FLAGS}")
add_executable(play-lookup ${PLAYGROUND_LOOKUP_SOURCE})

target_link_libraries(
    play-lookup
    ${${PRJ_PRX}_PLAY_LIBRARIES_PACK}
)

set(PLAYGROUND_REVERSEGEN_SOURCE
    reversegen/reversegen.cxx
)

message("${PRJ_PRX}: playground reversegen cxx flags:${CMAKE_CXX_FLAGS}")
add_executable(play-reversegen ${PLAYGROUND_REVERSEGEN_SOURCE})

target_link_libraries(
    play-reversegen
    ${${PRJ_PRX}_PLAY_LIBRARIES_PACK}
)