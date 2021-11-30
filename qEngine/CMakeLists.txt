cmake_minimum_required(VERSION 3.17)
project(qEngine)

set(CMAKE_CXX_STANDARD 17)

include_directories(inc)
include_directories(inc/angelscript)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(qEngine main.cpp src/qubit.cpp src/qubit.hpp src/common.hpp src/gates.cpp src/gates.hpp
        inc/angelscript/scriptstdstring/scriptstdstring.cpp
        inc/angelscript/scripthandle/scripthandle.cpp
        inc/angelscript/weakref/weakref.cpp
        inc/angelscript/scriptbuilder/scriptbuilder.cpp
        inc/angelscript/scriptarray/scriptarray.cpp
        inc/angelscript/scripthelper/scripthelper.cpp)

target_link_libraries(qEngine libangelscript.a)