cmake_minimum_required(VERSION 3.13)  # CMake version check
project(LN_CORELOOP)               # Create project "simple_example"

# Add main.cpp file of the project root directory as a source file

set(SOURCE_FILES src/main.c src/LuSEE_IO.c coreloop/cdi_interface.c coreloop/spectrometer_interface.c coreloop/core_loop.c)


# Add executable target with source files listed in SOURCE_FILES variable
include_directories(include)
include_directories(coreloop)

find_library(MATH_LIBRARY m)

add_executable(coreloop ${SOURCE_FILES})
target_link_libraries(coreloop PUBLIC ${MATH_LIBRARY})
set_property(TARGET coreloop PROPERTY C_STANDARD 99)