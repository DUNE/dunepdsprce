cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

project(protodune-dam VERSION 0.0.3)

set (CMAKE_CXX_STANDARD 14)

add_subdirectory(dam/source/cc/src)
add_subdirectory(dam/source/cc/ptd)
