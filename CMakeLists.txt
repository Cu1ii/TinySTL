cmake_minimum_required(VERSION 2.8.12)
project(TinySTL)


set(CMAKE_CXX_STANDARD 14)

add_executable(TinySTL
        TinySTL/util.h
        TinySTL/iterator.h
        TinySTL/type_traits.h
        TinySTL/algobase.h
        TinySTL/functional.h
        TinySTL/heap_algo.h
        TinySTL/construct.h
        TinySTL/allocator.h
        TinySTL/uninitialized.h
        TinySTL/memory.h
        TinySTL/algo.h
        tmp/list.h
        TinySTL/exceptdef.h
        TinySTL/list.h main.cpp
        Test)
