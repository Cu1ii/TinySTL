cmake_minimum_required(VERSION 3.21)
project(TinySTL)

set(CMAKE_CXX_STANDARD 14)

add_executable(TinySTL main.cpp TinySTL/algobase.h TinySTL/iterator.h TinySTL/type_traits.h)
