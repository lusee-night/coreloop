cmake_minimum_required(VERSION 3.13)  # CMake version check
project(LN_CORELOOP)               # Create project "simple_example"

# custom target
ADD_CUSTOM_TARGET(generate_h_from_md ALL COMMAND make -C ../pycoreloop)

# Add main.cpp file of the project root directory as a source file

set(SOURCE_FILES src/main.c src/LuSEE_IO.c src/LuSEE_SPI.c src/cdi_interface.c src/spectrometer_interface.c 
    src/calibrator_interface.c coreloop/core_loop.c coreloop/commanding.c coreloop/flash_interface.c coreloop/spectra_in.c 
    coreloop/spectra_out.c coreloop/housekeeping.c coreloop/gain.c coreloop/utils.c coreloop/sequencer.c
    coreloop/calibrator.c coreloop/fft.c coreloop/watchdogs.c)


# Add executable target with source files listed in SOURCE_FILES variable
include_directories(include)
include_directories(coreloop)
add_definitions(-DNOTREAL)
# Add compiler options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wno-format-security -Wno-address-of-packed-member")

add_compile_definitions(CORELOOP_ROOT="${CMAKE_SOURCE_DIR}")

find_library(MATH_LIBRARY m)

add_executable(coreloop ${SOURCE_FILES})
target_link_libraries(coreloop PUBLIC ${MATH_LIBRARY})
set_property(TARGET coreloop PROPERTY C_STANDARD 99)

add_executable(coding_test src/coding_test.c coreloop/utils.c)
target_link_libraries(coding_test PUBLIC ${MATH_LIBRARY})
set_property(TARGET coding_test PROPERTY C_STANDARD 99)

add_executable(coding_test_arr src/coding_test_arr.c coreloop/utils.c)
target_link_libraries(coding_test_arr PUBLIC ${MATH_LIBRARY})
set_property(TARGET coding_test_arr PROPERTY C_STANDARD 99)

add_library(cl_utils SHARED coreloop/utils.c)
set_property(TARGET cl_utils PROPERTY C_STANDARD 99)
