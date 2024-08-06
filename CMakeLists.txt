cmake_minimum_required(VERSION 3.13)  # CMake version check
project(LN_CORELOOP)               # Create project "simple_example"

# custom target
ADD_CUSTOM_TARGET(generate_h_from_md ALL COMMAND make -C ../pycoreloop)

# Add main.cpp file of the project root directory as a source file

set(SOURCE_FILES src/main.c src/LuSEE_IO.c src/LuSEE_SPI.c src/cdi_interface.c src/spectrometer_interface.c 
    coreloop/core_loop.c coreloop/commanding.c coreloop/flash_interface.c coreloop/spectra_in.c 
    coreloop/spectra_out.c coreloop/housekeeping.c coreloop/gain.c coreloop/sequencer.c coreloop/utils.c)


# Add executable target with source files listed in SOURCE_FILES variable
include_directories(include)
include_directories(coreloop)
add_definitions(-DNOTREAL)
# Add compiler options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format-security -Wno-address-of-packed-member")

find_library(MATH_LIBRARY m)

add_executable(coreloop ${SOURCE_FILES})
target_link_libraries(coreloop PUBLIC ${MATH_LIBRARY})
set_property(TARGET coreloop PROPERTY C_STANDARD 99)
