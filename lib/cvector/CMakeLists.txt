cmake_minimum_required(VERSION 3.27.4)
project(cvector C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
        src/cvector_core.c
        src/cvector_insert_erase.c
        src/cvector_resize.c
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC include)
