cmake_minimum_required(VERSION 3.0)
project(JNP1_2)

# Set default compile flag for GCC
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Stops on first error:
    set(CMAKE_CXX_FLAGS "-Wfatal-errors -std=c++17 -O0")
    #set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++17 -O0")
    set(CMAKE_C_FLAGS "-Wfatal-errors -std=c11 -O0")
    #set(CMAKE_C_FLAGS "-Wall -Wextra -std=c11 -O0")
endif ()

add_executable(scratchpad scratchpad.cc)
add_executable(test1 testy/encstrset_test1.c encstrset.cc)
add_executable(test2 testy/encstrset_test2.cc encstrset.cc)