cmake_minimum_required(VERSION 3.14.5)
project(neon_md)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_CXX_STANDARD 11)
file(GLOB SRC_FILE "src/*.cpp" "include/*.h" "include/fix/*.h")

add_executable(${PROJECT_NAME} ${SRC_FILE})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_DEF_FLAGS)

set(CMAKE_CXX_FLAGS "-Wall -Wunused-parameter -Wextra -ldl -std=c++11 -pthread -march=native -Wmissing-field-initializers -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(/usr/local/include/)
include_directories(/usr/include/)
# target_link_directories(${PROJECT_NAME} "/usr/local/lib64/libglog.a")
