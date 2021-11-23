cmake_minimum_required(VERSION 3.17)
project(qEngine)

set(CMAKE_CXX_STANDARD 17)

include_directories(inc)

add_executable(qEngine main.cpp src/qubit.cpp src/qubit.hpp src/common.hpp src/gates.cpp src/gates.hpp)