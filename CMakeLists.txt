cmake_minimum_required(VERSION 3.30)
project(myftp C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(lib/cvector)
add_subdirectory(core)
