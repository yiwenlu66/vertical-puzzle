cmake_minimum_required(VERSION 3.8)
project(vertical-puzzle-cli)

set(CMAKE_CXX_STANDARD 11)

include_directories(../core)

set(SOURCE_FILES main.cpp ../core/parsers.cpp ../core/utils.cpp ../core/constraint.cpp ../core/placeholder.cpp ../core/solver.cpp)
add_executable(vertical-puzzle-cli ${SOURCE_FILES})