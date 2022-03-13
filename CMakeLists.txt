cmake_minimum_required(VERSION 2.8.12)
project(TinySTL)

set(CMAKE_CXX_STANDARD 14)

add_executable(TinySTL main.cpp TinySTL/util.h TinySTL/iterator.h TinySTL/type_traits.h TinySTL/algobase.h)
