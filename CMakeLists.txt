cmake_minimum_required(VERSION 2.8.12)
project(TinySTL)


set(CMAKE_CXX_STANDARD 14)

add_executable(
        TinySTL
        Test main.cpp
        TinySTL/stack.h
        Test/stack_test.h
        Test/vector_test.h
        TinySTL/vector.h
        TinySTL/queue.h
        TinySTL/rb_tree.h
        TinySTL/set.h
        Extend/segtree.h
        Extend/internal_bit.hpp
        Test/segtree_test.h)
