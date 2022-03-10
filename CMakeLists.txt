cmake_minimum_required(VERSION 3.21)
project(TinySTL)

set(CMAKE_CXX_STANDARD 14)

add_executable(TinySTL main.cpp TinySTL/util.h TinySTL/iterator.h TinySTL/type_traits.h TinySTL/algobase.h)
