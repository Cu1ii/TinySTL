//
// Created by Cu1 on 2022/3/7.
//

#ifndef TINYSTL_UTIL_H
#define TINYSTL_UTIL_H

#include <cstddef>
#include "type_traits.h"

namespace tstl
{
    // move
    template <class T>
    typename std::remove_reference<T>::type move(T&& arg) noexcept
    {
        return static_cast<typename std::remove_reference<T>::type&&>(arg);
    }

    //forward
    template <class T>
    T&& forward(typename std::remove_reference<T>::type& arg) noexcept
    {
        return static_cast<T&&>(arg);
    }

    template <class T>
    T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
    {
        static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
        return static_cast<T&&>(arg);
    }

    // swap
    template <class Tp>
    void swap(Tp& lhs, Tp& rhs)
    {
        Tp tmp(tstl::move(lhs));
        lhs = tstl::move(rhs);
        rhs = tstl::move(tmp);
    }

    // 将前两个迭代器之间的元素交换到以第三个迭代器为开始的地址, 返回交换完成后第二个迭代器的结束位置
    template <class ForwardIter1, class ForwardIter2>
    ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2)
    {
        for (; first1 != last1; ++first1, (void) ++first2)
            tstl::swap(*first1, *first2);
        return first2;
    }

    // 对于数组交换的偏特化
    template <class Tp, size_t N>
    void swap(Tp(&a)[N], Tp(&b)[N])
    {
        tstl::swap_range(a, a + N, b);
    }

    //-----------------------------------------pair-------------------------------------------------
    /**
     * @name pair
     * @details: 两个模板参数分别表示两个数据类型, 使用 first 和 second 来分别表示第一个数据和第二个数据
     */
     template <class T1, class T2>
     struct pair
     {
         typedef T1     first_type;
         typedef T2     second_type;

         first_type     first; // 第一个数据
         second_type    second; // 第二个数据


         // 默认构造
         template <class Other1 = T1, class Other2 = T2,
                 typename = typename std::enable_if<
                         std::is_default_constructible<Other1>::value &&
                         std::is_default_constructible<Other2>::value, void>::type>
                         constexpr pair()
                         : first(), second()
         {
         }

         // 该类型的可转化隐式构造
         template <class U1 = T1, class U2 = T2,
                 typename std::enable_if<
                 std::is_copy_constructible<U1>::value &&
                 std::is_copy_constructible<U2>::value &&
                 std::is_convertible<const U1&, T1>::value &&
                 std::is_convertible<const U2&, T2>::value, int>::type = 0>
                         constexpr pair(const T1& a, const T2& b)
                         : first(a), second(b)
         {
         }

         template <class U1 = T1, class U2 = T2,
                 typename std::enable_if<
                 std::is_copy_constructible<U1>::value &&
                 std::is_copy_constructible<U2>::value &&
                 (!std::is_convertible<const U1&, T1>::value ||
                 !std::is_convertible<const U2&, T2>::value), int>::type = 0>
                         explicit constexpr pair(const T1& a, const T2& b)
                         : first(a), second(b)
         {
         }

         pair(const pair& rhs) = default;
         pair(pair&& rhs) = default;

         // 利用其他可隐式转化为 <T1, T2> 类型的元素构造 pair<T1, T2>
         template <class Other1, class Other2,
                 typename std::enable_if<
                 std::is_constructible<T1, Other1>::value &&
                 std::is_constructible<T2, Other2>::value &&
                 std::is_convertible<Other1&&, T1>::value &&
                 std::is_convertible<Other2&&, T2>::value, int>::type = 0>
                         constexpr pair(Other1&& a, Other2&& b)
                         : first(tstl::forward<Other1>(a)),
                         second(tstl::forward<Other2>(b))
         {
         }

         // 其他类型的 pair 显式构造
         template <class Other1, class Other2,
                 typename std::enable_if<
                 std::is_constructible<T1, Other1>::value &&
                 std::is_constructible<T2, Other2>::value &&
                 (!std::is_convertible<Other1, T1>::value ||
                  !std::is_convertible<Other2, T2>::value), int>::type = 0>
                          explicit constexpr pair(Other1&& a, Other2&& b)
                          : first(tstl::forward<Other1>(a)),
                          second(tstl::forward<Other2>(b))
         {
         }

         // 将 pair<Other1, Other2> 类型隐式构造为 pair<T1, T2>
         template <class Other1, class Other2,
                 typename std::enable_if<
                 std::is_constructible<T1, const Other1&>::value &&
                 std::is_constructible<T2, const Other2&>::value &&
                 std::is_convertible<const Other1&, T1>::value &&
                 std::is_convertible<const Other2&, T2>::value, int>::type = 0>
                         constexpr pair(const pair<Other1, Other2>& other)
                         : first(other.first),
                         second(other.second)
         {
         }

         // 将 pair<Other1, Other2> 类型显式构造为 pair<T1, T2>
         template <class Other1, class Other2,
                 typename std::enable_if<
                 std::is_constructible<T1, Other1>::value &&
                 std::is_constructible<T2, Other2>::value &&
                 (!std::is_convertible<Other1, T1>::value ||
                  !std::is_convertible<Other2, T2>::value), int>::type = 0>
                          explicit constexpr pair(pair<Other1, Other2>&& other)
                          : first(tstl::forward<Other1>(other.first)),
                          second(tstl::forward<Other2>(other.second))
         {
         }

         // 重载运算符
         pair& operator=(const pair& rhs)
         {
             if (this != &rhs)
             {
                 first = rhs.first;
                 second = rhs.second;
             }
             return *this;
         }

         // move 版本 = 运算符
         pair& operator=(const pair& rhs)
         {
             if (this != &rhs)
             {
                 first = tstl::move(rhs.first);
                 second = tstl::move(rhs.second);
             }
             return *this;
         }

         template <class Other1, class Other2>
         pair& operator=(const pair<Other1, Other1>& other)
         {
             first = other.first;
             second = other.second;
             return *this;
         }

         template <class Other1, class Other2>
         pair& operator=(const pair<Other1, Other1>&& other)
         {
             first = tstl::move(other.first);
             second = tstl::move(other.second);
             return *this;
         }

         //----------------------------------------------------
         ~pair() = default;

         void swap(pair& other)
         {
             if (this != &other)
             {
                 tstl::swap(first, other.first);
                 tstl::swap(second, other.second);
             }
         }
     };

     // 全局重载操作符
     template <class T1, class T2>
     bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
     {
         return lhs.first == rhs.first && lhs.second == rhs.second;
     }

     template <class T1, class T2>
     bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
     {
         if (lhs.first == rhs.first)
             return lhs.second < rhs.second;
         return lhs.first < rhs.first;
     }

     template <class T1, class T2>
     bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
     {
         return !(lhs == rhs);
     }

     template <class T1, class T2>
     bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
     {
         return rhs < lhs;
     }

     template <class T1, class T2>
     bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
     {
         return !(rhs < lhs)
     }

     template <class T1, class T2>
     bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
     {
         return !(lhs < rhs);
     }

     // 全局重载 swap
     template <class T1, class T2>
     void swap(pair<T1, T2>& a, pair<T1, T2>& b)
     {
         a.swap(b);
     }

     //提供构建 pair 的全局函数
     template <class T1, class T2>
     pair<T1, T2> make_pair(T1&& first, T2&& second)
     {
         return pair<T1, T2>(tstl::forward<T1>(first), tstl::forward<T2>(second));
     }
}


#endif //TINYSTL_UTIL_H
