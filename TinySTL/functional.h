//
// Created by Cu1 on 2022/3/14.
//

#ifndef TINYSTL_FUNCTIONAL_H
#define TINYSTL_FUNCTIONAL_H

// 该头文件包含了 TinySTL 的仿函数以及 Hash 函数

#include <cstddef>

namespace tstl
{
    // 定义一元函数的参数型别和返回值型别
    template <class Arg, class Result>
    struct unarg_function
    {
        typedef Arg             argument_type;
        typedef Result          result_type;
    };

    // 定义二元函数参数类型和返回值类型
    template <class Arg1, class Arg2, class Result>
    struct binary_function
    {
        typedef Arg1            first_argument_type;
        typedef Arg2            second_argument_type;
        typedef Result          result_type;
    };

    // 仿函数 加法
    template <class T>
    struct plus : public binary_function<T, T, T>
    {
        T operator()(const T& a, const T& b) const { return a + b; }
    };

    // 仿函数 减法
    template <class T>
    struct minus : public binary_function<T, T, T>
    {
        T operator()(const T& a, const T& b) const { return a - b; }
    };

    // 仿函数 乘法
    template <class T>
    struct multiplies : public binary_function<T, T, T>
    {
        T operator()(const T& a, const T& b) const { return a * b; }
    };

    // 仿函数 除法
    template <class T>
    struct divides : public binary_function<T, T, T>
    {
        T operator()(const T& a, const T& b) const { return x / y; }
    };

    // 仿函数 取模
    template <class T>
    struct modulus : public binary_function<T, T, T>
    {
        T operator()(const T& a, const T& b) const { return a % b; }
    };

    // 仿函数 取反
    template <class T>
    struct negate : public unarg_function<T, T>
    {
        T operator()(const T& x) const { return  -x; }
    };

    // 加法的证同对象
    template <class T>
    T identity_element(plus<T>) { return T(0); }

    //乘法的正同元素
    template <class  T>
    T identity_element(multiplies<T>) { return T(1); }


    // 仿函数 等于
    template <class T>
    struct equal_to : public binary_function<T, T, bool>
    {
        bool  operator()(const T& a, const T& b) const { return a == b; }
    };

    // 仿函数 不等于
    template <class T>
    struct not_equal_to : public binary_function<T, T, bool>
    {
        bool  operator()(const T& a, const T& b) const { return a != b; }
    };

    //仿函数 大于
    template <class T>
    struct greater : public binary_function<T, T, bool>
    {
        bool operator()(const T& a, const T& b) { return a > b; }
    };

    // 仿函数 小于
    template <class T>
    struct less : public binary_function<T, T, bool>
    {
        bool operator()(const T& a, const T& b) { return a < b; }
    };

    //仿函数 大于等于
    template <class T>
    struct greater_equal : public binary_function<T, T, bool>
    {
        bool operator()(const T& a, const T& b) { return a >= b; }
    };

    // 仿函数 小于等于
    template <class T>
    struct less_equal : public binary_function<T, T, bool>
    {
        bool operator()(const T& a, const T& b) { return a <= b; }
    };

    // 仿函数 逻辑与
    template <class T>
    struct logical_or :public binary_function<T, T, bool>
    {
        bool operator()(const T& x, const T& y) const { return x && y; }
    };

    // 仿函数 逻辑或
    template <class T>
    struct logical_or :public binary_function<T, T, bool>
    {
        bool operator()(const T& x, const T& y) const { return x || y; }
    };


    // 仿函数 逻辑非
    template <class T>
    struct logical_not :public unarg_function<T, bool>
    {
        bool operator()(const T& x) const { return !x; }
    };

    // 证同仿函数 不会改变元素, 返回本身
    template <class T>
    struct identity : public unarg_function<T, const T&>
    {
        const T& operator()(const T& x) const { return x; }
    };

    // 选择 仿函数 接受一个 pair, 返回第一个元素
    template <class Pair>
    struct selectfirst : public unarg_function<Pair, typename Pair::first_type>
    {
        const typename Pair::first_type& operator()(const Pair& x) const
        {
            return x.first;
        }
    };

    // 选择 仿函数 接受一个 pair, 返回第一个元素
    template <class Pair>
    struct selectsecond : public unarg_function<Pair, typename Pair::second_type>
    {
        const typename Pair::second_type& operator()(const Pair& x) const
        {
            return x.second;
        }
    };

}


#endif //TINYSTL_FUNCTIONAL_H
